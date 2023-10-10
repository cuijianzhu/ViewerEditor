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
    vcg::tri::RequirePerVertexTexCoord(m_mesh);
    vcg::tri::RequirePerFaceWedgeTexCoord(m_mesh);
    vcg::tri::io::Importer<MyMesh>::Open(m_mesh, path_.c_str());
    boost::filesystem::path path = path_;
    m_rootDir                    = path.parent_path().string();
    updateOSGNode();
}

void Mesh::updateOSGNode()
{
    // 删除所有Drawable
    removeDrawables(0, getNumDrawables());

    osg::ref_ptr<osg::Vec3Array> vertices  = new osg::Vec3Array;
    std::vector<osg::ref_ptr<osg::DrawElementsUInt>> vdrawElements;
    std::vector<osg::ref_ptr<osg::Vec2Array>> vtexCoords;
    std::vector <osg::ref_ptr<osg::Geometry>>        vgeometry;
    for (size_t i = 0; i < m_mesh.textures.size(); i++) {
        osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array;
        texCoords->resize(m_mesh.VertexNumber());
        vtexCoords.push_back(texCoords);

        osg::ref_ptr<osg::DrawElementsUInt> drawElements =
            new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
        vdrawElements.push_back(drawElements);
        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
        vgeometry.push_back(geometry);
    }
    
    for (size_t i = 0; i < m_mesh.VertexNumber(); i++) {
        auto& v = m_mesh.vert[i];
        auto& f = m_mesh.face[i];
        vertices->push_back(osg::Vec3(v.P().X(), v.P().Y(), v.P().Z()));
        for (size_t j = 0; j < vtexCoords.size(); j++) {
            vtexCoords[j]->at(i) = osg::Vec2f(0.0f, 0.0f);
        }
    }

    for (auto f : m_mesh.face) {
        if (!f.IsD()) {
            for (size_t i = 0; i < 3; i++) {
                vdrawElements[f.V(i)->T().N()]->push_back(vcg::tri::Index(m_mesh, f.V(i)));
                vtexCoords[f.V(i)->T().N()]->at(vcg::tri::Index(m_mesh, f.V(i))) =
                    osg::Vec2f(f.WT(i).u(), f.WT(i).v());
            }
        }
    }

    for (size_t i = 0; i < vgeometry.size(); i++) {
        vgeometry[i]->setVertexArray(vertices.get());
        vgeometry[i]->addPrimitiveSet(vdrawElements[i]);
        vgeometry[i]->setTexCoordArray(0, vtexCoords[i]);

        // 纹理
        boost::filesystem::path textPath = boost::filesystem::path(m_rootDir) / m_mesh.textures[i];
        osg::ref_ptr<osg::StateSet> stateset = vgeometry[i]->getOrCreateStateSet();
        auto            image   = osgDB::readImageFile(textPath.generic_path().generic_string());
        osg::Texture2D* texture = new osg::Texture2D(image);
        stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

        // 材质
        osg::Material* osg_material = new osg::Material;
        stateset->setAttribute(osg_material);
        osg_material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        osg_material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));
        osg_material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        
        addDrawable(vgeometry[i]);
    }
}
