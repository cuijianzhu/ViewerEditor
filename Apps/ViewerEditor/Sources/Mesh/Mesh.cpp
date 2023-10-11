#include "Mesh.h"
#include <wrap/io_trimesh/import.h>
#include <boost/filesystem/path.hpp>
#include <osg/Array>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Material>
#include <vector>
#include <iostream>
#include <unordered_map>
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

Mesh::Mesh() {}

void Mesh::read(const std::string& path_) {
    m_mesh.Clear();
    vcg::tri::RequirePerVertexTexCoord(m_mesh);
    vcg::tri::RequirePerFaceWedgeTexCoord(m_mesh);
    vcg::tri::io::Importer<MyMesh>::Open(m_mesh, path_.c_str());
    boost::filesystem::path path = path_;
    m_rootDir                    = path.parent_path().string();
    updateOSGNode();
}

void Mesh::rectanglePick(vcg::Box3<MyMesh::ScalarType> box_, osg::Matrix vpmMatrix_, bool isInvertSelection_)
{
    for (auto& f : m_mesh.face) {
        if (f.IsD()) continue;
        auto v0 = osg::Vec3(f.V(0)->P().X(), f.V(0)->P().Y(), f.V(0)->P().Z()) * vpmMatrix_;
        auto v1 = osg::Vec3(f.V(1)->P().X(), f.V(1)->P().Y(), f.V(1)->P().Z()) * vpmMatrix_;
        auto v2 = osg::Vec3(f.V(2)->P().X(), f.V(2)->P().Y(), f.V(2)->P().Z()) * vpmMatrix_;
        
        vcg::Point3f vcgv0(v0.x(), v0.y(), v0.z());
        vcg::Point3f vcgv1(v1.x(), v1.y(), v1.z());
        vcg::Point3f vcgv2(v2.x(), v2.y(), v2.z());
        if (vcg::IntersectionTriangleBox(box_, vcgv0, vcgv1, vcgv2)) {
            if (isInvertSelection_) {
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
    std::vector<osg::ref_ptr<osg::Vec3Array>>       vvertices;
    std::vector<osg::ref_ptr<osg::DrawElementsUInt>> vdrawElements;
    std::vector<osg::ref_ptr<osg::Vec2Array>> vtexCoords;
    std::vector <osg::ref_ptr<osg::Geometry>>        vgeometry;
    std::vector<TexCoordMap>                         vtexcoordMap;

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
                auto& v = f.V(i);
                short                   nTex        = f.WT(i).n();
                std::pair<float, float> texCoord =
                    std::make_pair(f.WT(i).u(), f.WT(i).v());
                auto&                   texcoordMap = vtexcoordMap[nTex];
                if (texcoordMap.find(texCoord) == texcoordMap.end()) {
                    osg::Vec3 vertex =
                        osg::Vec3(v->P().X(), v->P().Y(), v->P().Z());
                    std::tuple<short, osg::Vec3, size_t> tuple(
                        f.WT(i).n(), vertex, texcoordMap.size());
                    texcoordMap[texCoord] = tuple;
                    vvertices[nTex]->push_back(vertex);
                    vtexCoords[nTex]->push_back({texCoord.first, texCoord.second});
                }

                auto      tuple         = texcoordMap[texCoord];
                size_t    texCoordIndex = std::get<2>(tuple);

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
        if (m_mesh.textures.size()!=0) {
            boost::filesystem::path textPath =
                boost::filesystem::path(m_rootDir) / m_mesh.textures[i];
            osg::ref_ptr<osg::Image>    image =
                osgDB::readImageFile(textPath.generic_path().generic_string());
            osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
            stateset->setTextureAttributeAndModes(0, texture);
        }
        
        // 材质
        osg::ref_ptr<osg::Material> osg_material = new osg::Material;
        stateset->setAttribute(osg_material);
        osg_material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        osg_material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));
        osg_material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->addDrawable(vgeometry[i]);
        addChild(geode);
    }
}
