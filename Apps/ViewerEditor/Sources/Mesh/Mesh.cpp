#include "Mesh.h"
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <unordered_map>
#include <vector>
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>
namespace std {
template<> struct hash<std::pair<float, float>>
{
    size_t operator()(std::pair<float, float> const& vertex) const
    {
        // 使用异或运算对每个分量进行哈希
        std::size_t h1 = std::hash<float>{}(vertex.first);
        std::size_t h2 = std::hash<float>{}(vertex.second);

        // 结合哈希值
        return h1 ^ (h2 << 1);
    }
};
}   // namespace std

Mesh::Mesh() {
    osg::ref_ptr<osg::StateSet>      stateset   = new osg::StateSet;
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    setStateSet(stateset);
}

void Mesh::read(const std::string& path_)
{
    m_mesh.Clear();
    vcg::tri::RequirePerVertexTexCoord(m_mesh);
    vcg::tri::RequirePerFaceWedgeTexCoord(m_mesh);
    vcg::tri::io::Importer<MyMesh>::Open(m_mesh, path_.c_str());
    boost::filesystem::path path = path_;
    m_rootDir                    = path.parent_path().string();
    setName("meshNode");
    updateOSGNode();
}

void Mesh::write(const std::string& path_) {
    vcg::tri::io::Exporter<MyMesh>::Save(
        m_mesh, path_.c_str(), vcg::tri::io::Mask::IOM_WEDGTEXCOORD);
}

void Mesh::pickSphere(osg::Vec3 center_, float raduis_, bool isIvert)
{
    float   distance = std::numeric_limits<float>::max();
    MyFace* minFace;
    for (auto& f : m_mesh.face) {
        if (f.IsD()) continue;
        vcg::Point3f                     center(center_.x(), center_.y(), center_.z());
        vcg::Sphere3<MyMesh::ScalarType> sphere(center, raduis_);
        vcg::Point3f                     witness;
        auto                             fcenter = vcg::Barycenter(f);

        if (vcg::Distance(fcenter, center) < raduis_ || vcg::Distance(f.P(0), center) < raduis_ ||
            vcg::Distance(f.P(1), center) < raduis_ || vcg::Distance(f.P(2), center) < raduis_) {
            if (isIvert) {
                f.ClearS();
            }
            else {
                f.SetS();
            }
            continue;
        }

        if (vcg::IntersectionSphereTriangle(sphere, f, witness) /* && distance < raduis_*4*/) {
            float pd0 = vcg::Distance(f.P(0), witness);
            float pd1 = vcg::Distance(f.P(1), witness);
            float pd2 = vcg::Distance(f.P(2), witness);
            float fwd = vcg::Distance(fcenter, witness);
            if (fwd > pd0 || fwd > pd1 || fwd > pd2) continue;

            if (vcg::Distance(center, witness) > raduis_) continue;

            // std::cout << "witness : " << witness << std::endl;
            if (isIvert) {
                f.ClearS();
            }
            else {
                f.SetS();
            }
        }
    }
}

void Mesh::updateOSGNode()
{
    // 删除所有Drawable
    removeChild(0, getNumChildren());
    typedef std::unordered_map<std::pair<float, float>, std::tuple<short, osg::Vec3, size_t>>
                                                     TexCoordMap;
    std::vector<osg::ref_ptr<osg::Vec3Array>>        vvertices;
    std::vector<osg::ref_ptr<osg::DrawElementsUInt>> vdrawElements;
    std::vector<osg::ref_ptr<osg::Vec2Array>>        vtexCoords;
    std::vector<osg::ref_ptr<osg::Geometry>>         vgeometry;
    std::vector<TexCoordMap>                         vtexcoordMap;
    m_TexNo = m_mesh.textures.size();

    for (size_t i = 0; i < 1 || i < m_mesh.textures.size(); i++) {
        osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
        vtexCoords.push_back(texCoords);

        osg::ref_ptr<osg::DrawElementsUInt> drawElements =
            new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
        vdrawElements.push_back(drawElements);
        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
        vgeometry.push_back(geometry);

        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        vvertices.push_back(vertices);
    }
    vtexcoordMap.resize(m_mesh.textures.size());


    for (auto& f : m_mesh.face) {
        if (!f.IsD()) {
            for (size_t i = 0; i < 3; i++) {
                auto&                   v           = f.V(i);
                short                   nTex        = f.WT(i).n();
                std::pair<float, float> texCoord    = std::make_pair(f.WT(i).u(), f.WT(i).v());
                auto&                   texcoordMap = vtexcoordMap[nTex];
                if (texcoordMap.find(texCoord) == texcoordMap.end()) {
                    osg::Vec3 vertex = osg::Vec3(v->P().X(), v->P().Y(), v->P().Z());
                    std::tuple<short, osg::Vec3, size_t> tuple(
                        f.WT(i).n(), vertex, texcoordMap.size());
                    texcoordMap[texCoord] = tuple;
                    vvertices[nTex]->push_back(vertex);
                    vtexCoords[nTex]->push_back({texCoord.first, texCoord.second});
                }

                auto   tuple         = texcoordMap[texCoord];
                size_t texCoordIndex = std::get<2>(tuple);

                vdrawElements[nTex]->push_back(texCoordIndex);
            }
        }
    }


    for (size_t i = 0; i < vgeometry.size(); i++) {
        vgeometry[i]->setVertexArray(vvertices[i]);
        vgeometry[i]->addPrimitiveSet(vdrawElements[i]);
        vgeometry[i]->setTexCoordArray(0, vtexCoords[i]);
        osg::ref_ptr<osg::StateSet> stateset = vgeometry[i]->getOrCreateStateSet();

        // 纹理
        if (m_mesh.textures.size() != 0) {
            boost::filesystem::path textPath =
                boost::filesystem::path(m_rootDir) / m_mesh.textures[i];
            osg::ref_ptr<osg::Image> image =
                osgDB::readImageFile(textPath.generic_path().generic_string());
            osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
            stateset->setTextureAttributeAndModes(0, texture);
        }

        // 材质
        osg::ref_ptr<osg::Material> osg_material = new osg::Material;
        stateset->setAttribute(osg_material);
        stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        osg_material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        osg_material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        osg_material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->addDrawable(vgeometry[i]);
        addChild(geode);
    }
}

void Mesh::deleteFace()
{
    for (auto& f : m_mesh.face) {
        if (f.IsS()) {
            f.SetD();
        }
    }

    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m_mesh);
    vcg::tri::Clean<MyMesh>::RemoveSmallConnectedComponentsSize(m_mesh, 100);
    updateOSGNode();
}

void Mesh::removeSmall() {
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m_mesh);
    vcg::tri::Clean<MyMesh>::RemoveSmallConnectedComponentsSize(m_mesh, 100);
    updateOSGNode();
}
