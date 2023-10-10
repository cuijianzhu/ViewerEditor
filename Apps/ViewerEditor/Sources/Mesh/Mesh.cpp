#include "Mesh.h"
#include <wrap/io_trimesh/import.h>
#include <boost/filesystem/path.hpp>
#include <osg/Array>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Material>
#include <vector>
#include <iostream>
Mesh::Mesh() {}

void Mesh::read(const std::string& path_) {
    int loadmask = vcg::tri::io::Mask::IOM_VERTTEXCOORD;
    vcg::tri::io::Importer<MyMesh>::Open(m_mesh, path_.c_str(), loadmask);
    boost::filesystem::path path = path_;
    m_rootDir                    = path.parent_path().string();
    m_geometry = new osg::Geometry;
    addDrawable(m_geometry);
    updateOSGNode();
}

void Mesh::updateOSGNode()
{
    vcg::tri::RequirePerVertexTexCoord(m_mesh);
    osg::ref_ptr<osg::Vec3Array> vertices  = new osg::Vec3Array;
    osg::ref_ptr<osg::DrawElementsUInt> drawElements =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
    std::vector<osg::ref_ptr<osg::Vec2Array>> vtexCoords;
    for (size_t i = 0; i < m_mesh.textures.size(); i++) {
        osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
        vtexCoords.push_back(texCoords);
    }
    
    for (size_t i = 0; i < m_mesh.VertexNumber(); i++) {
        auto& v = m_mesh.vert[i];
        auto& f = m_mesh.face[i];
        vertices->push_back(osg::Vec3(v.P().X(), v.P().Y(), v.P().Z()));
        vtexCoords[0]->push_back(osg::Vec2(v.T().u(), v.T().v()));
    }

    drawElements->clear();
    for (auto f : m_mesh.face) {
        if (!f.IsD()) {
            for (size_t i = 0; i < 3; i++) {
                drawElements->push_back(vcg::tri::Index(m_mesh, f.V(i)));
            }
        }
    }

    m_geometry->setVertexArray(vertices.get());
    m_geometry->addPrimitiveSet(drawElements);
    m_geometry->setTexCoordArray(0, vtexCoords[0]);

    /*for (size_t i = 0; i < vtexCoords.size(); i++) {
        m_geometry->setTexCoordArray(0, vtexCoords[i]);
    }*/
    boost::filesystem::path     textPath = boost::filesystem::path(m_rootDir) / m_mesh.textures[0];
    osg::ref_ptr<osg::StateSet> stateset = m_geometry->getOrCreateStateSet();
    auto image = osgDB::readImageFile(textPath.generic_path().generic_string());
    osg::Texture2D* texture = new osg::Texture2D(image);
    stateset->setTextureAttributeAndModes(
        0, texture, osg::StateAttribute::ON);   // 0 表示第一个纹理单元
    //for (size_t i = 0; i < m_mesh.textures.size(); i++) {
    //    boost::filesystem::path textPath = boost::filesystem::path(m_rootDir) / m_mesh.textures[i];
    //    osg::ref_ptr<osg::StateSet> stateset = m_geometry->getOrCreateStateSet();
    //    stateset->setTextureAttributeAndModes(0, texture.get());   // 0 表示第一个纹理单元
    //    std::cout << "texture " << i << " " << textPath << std::endl;
    //}

    osg::Material* osg_material = new osg::Material;
    stateset->setAttribute(osg_material);
    osg_material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    osg_material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));
    osg_material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    m_geometry->dirtyDisplayList();
}
