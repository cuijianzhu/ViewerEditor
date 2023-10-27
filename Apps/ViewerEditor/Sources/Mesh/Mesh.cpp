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
#include <osgUtil/SmoothingVisitor>
#include <vcg/complex/algorithms/smooth.h>
#include <QImage>
#include <chrono>
#include <functional>
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
    withTexture();
}

void Mesh::read(const std::string& path_)
{
    m_mesh.Clear();
    vcg::tri::RequirePerVertexTexCoord(m_mesh);
    vcg::tri::RequirePerFaceWedgeTexCoord(m_mesh);
    vcg::tri::io::Importer<MyMesh>::Open(m_mesh, path_.c_str());
    vcg::tri::Append<MyMesh, MyMesh>::MeshCopy(m_originMesh, m_mesh);
    boost::filesystem::path path = path_;
    m_rootDir                    = path.parent_path().string();
    setName("meshNode");
    m_originFaceNumber = m_mesh.face.size();
    QImage grayImage(256, 256, QImage::Format_RGB32);
    grayImage.fill(Qt::gray);
    boost::filesystem::path grayImagePath = path.parent_path() / "gray.png";
    grayImage.save(QString::fromLocal8Bit(grayImagePath.generic_path().generic_string().c_str()));
    m_mesh.textures.push_back("gray.png");
    m_originTextureNumber = m_mesh.textures.size();
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
                for (size_t k = 0; k < 3; k++) {
                    f.V(k)->ClearS();
                }
            }
            else {
                f.SetS();
                for (size_t k = 0; k < 3; k++) {
                    f.V(k)->SetS();
                }
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


    for (int fn = 0; fn < m_mesh.face.size(); fn++) {
        auto& f = m_mesh.face[fn];
        if (!f.IsD())
        {
            for (size_t i = 0; i < 3; i++) {
                auto                   v           = f.V(i);
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
        if (m_withTexture) {
            vgeometry[i]->setTexCoordArray(0, vtexCoords[i]);
        }
        osg::ref_ptr<osg::StateSet> stateset = vgeometry[i]->getOrCreateStateSet();

        // 纹理
        if (m_mesh.textures.size() != 0) {
            boost::filesystem::path textPath =
                boost::filesystem::path(m_rootDir) / m_mesh.textures[i];
            osg::ref_ptr<osg::Image> image =
                osgDB::readImageFile(textPath.generic_path().generic_string());
            osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
            if (m_withTexture) {
                stateset->setTextureAttributeAndModes(0, texture);
            }
        }
        vgeometry[i]->setUseVertexBufferObjects(true);

        if (m_withTexture) {
            // 材质
            osg::ref_ptr<osg::Material> osg_material = new osg::Material;
            stateset->setAttribute(osg_material);
            stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
            osg_material->setAmbient(osg::Material::FRONT_AND_BACK,
            osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
            osg_material->setDiffuse(osg::Material::FRONT_AND_BACK,
            osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
            osg_material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        osgUtil::SmoothingVisitor::smooth(*vgeometry[i]);
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

void Mesh::noTexute() {
    m_withTexture = false;
    osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    setStateSet(stateset);
    updateOSGNode();
}

void Mesh::withTexture() {
    m_withTexture = true;
    osg::ref_ptr<osg::StateSet>      stateset   = new osg::StateSet;
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    setStateSet(stateset);
    updateOSGNode();
}

void Mesh::smooth() {
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(m_mesh);
    vcg::tri::UpdateNormal<MyMesh>::PerFaceNormalized(m_mesh);
    vcg::tri::Smooth<MyMesh>::VertexCoordTaubin(m_mesh, 20, 0.7, -0.7, true);
    updateOSGNode();
}

void Mesh::originRender() {
    // 删除所有Drawable
    removeChild(0, getNumChildren());
    typedef std::unordered_map<std::pair<float, float>, std::tuple<short, osg::Vec3, size_t>>
                                                     TexCoordMap;
    std::vector<osg::ref_ptr<osg::Vec3Array>>        vvertices;
    std::vector<osg::ref_ptr<osg::DrawElementsUInt>> vdrawElements;
    std::vector<osg::ref_ptr<osg::Vec2Array>>        vtexCoords;
    std::vector<osg::ref_ptr<osg::Geometry>>         vgeometry;
    std::vector<TexCoordMap>                         vtexcoordMap;

    for (size_t i = 0; i < 1 || i < m_originMesh.textures.size(); i++) {
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
    vtexcoordMap.resize(m_originMesh.textures.size());


    for (int fn = 0; fn < m_originMesh.face.size(); fn++) {
        auto& f = m_originMesh.face[fn];
        if (!f.IsD()) {
            for (size_t i = 0; i < 3; i++) {
                auto                    v        = f.V(i);
                short                   nTex     = f.WT(i).n();
                std::pair<float, float> texCoord = std::make_pair(f.WT(i).u(), f.WT(i).v());

                auto& texcoordMap = vtexcoordMap[nTex];
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
        if (m_withTexture) {
            vgeometry[i]->setTexCoordArray(0, vtexCoords[i]);
        }
        osg::ref_ptr<osg::StateSet> stateset = vgeometry[i]->getOrCreateStateSet();

        // 纹理
        if (m_originMesh.textures.size() != 0) {
            boost::filesystem::path textPath =
                boost::filesystem::path(m_rootDir) / m_originMesh.textures[i];
            osg::ref_ptr<osg::Image> image =
                osgDB::readImageFile(textPath.generic_path().generic_string());
            osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
            if (m_withTexture) {
                stateset->setTextureAttributeAndModes(0, texture);
            }
        }
        vgeometry[i]->setUseVertexBufferObjects(true);

        if (m_withTexture) {
            // 材质
            osg::ref_ptr<osg::Material> osg_material = new osg::Material;
            stateset->setAttribute(osg_material);
            stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
            osg_material->setAmbient(osg::Material::FRONT_AND_BACK,
                                     osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
            osg_material->setDiffuse(osg::Material::FRONT_AND_BACK,
                                     osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
            osg_material->setEmission(osg::Material::FRONT_AND_BACK,
                                      osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        osgUtil::SmoothingVisitor::smooth(*vgeometry[i]);
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->addDrawable(vgeometry[i]);
        addChild(geode);
    }
}

void Mesh::newRender() {
    m_isOrigin = false;
    updateOSGNode();
}

std::string Mesh::reTexture(osg::Matrixd vpmMatrix)
{

    auto now = std::chrono::system_clock::now();

    // 转换为时间戳（以秒为单位）
    auto now_sec = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    // 计算 hash 值
    std::hash<decltype(now_sec)> hasher;
    auto                         hash = hasher(now_sec);
    std::string                  texturePath = std::to_string(now_sec) + ".png";

    for (auto& f : m_mesh.face) {
        if (f.IsD()) continue;
        for (size_t k = 0; k < 3; k++) {
            if (f.WT(k).n() == (m_originTextureNumber - 1)) {
                auto uvCoord =
                    osg::Vec3(f.V(k)->P().X(), f.V(k)->P().Y(), f.V(k)->P().Z()) * vpmMatrix;
                uvCoord[0]                        = (uvCoord[0] + 1.0f) * 0.5f;
                uvCoord[1]                        = (uvCoord[1] + 1.0f) * 0.5f;
                f.WT(k).u() = uvCoord[0];
                f.WT(k).v() = uvCoord[1];
                f.WT(k).n()                       = m_mesh.textures.size();
            }
            ;
        }
    }
    m_mesh.textures.push_back(texturePath);
    return texturePath;
}
