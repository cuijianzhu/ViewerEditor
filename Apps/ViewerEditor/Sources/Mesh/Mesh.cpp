#include "Mesh.h"
#include <wrap/io_trimesh/import.h>
#include <boost/filesystem/path.hpp>
#include <osg/Array>
#include <osg/Geometry>
#include <osg/Geode>
#include <vector>
#include <iostream>
Mesh::Mesh() {}

void Mesh::read(const std::string& path_) {
    vcg::tri::io::Importer<MyMesh>::Open(m_mesh, path_.c_str());
    m_geometry = new osg::Geometry;
    addDrawable(m_geometry);
    updateOSGNode();
}

void Mesh::updateOSGNode()
{
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
        vtexCoords[v.T().N()]->push_back(osg::Vec2(v.T().U(), v.T().V()));
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
    for (size_t i = 0; i < vtexCoords.size(); i++) {
        m_geometry->setTexCoordArray(i, vtexCoords[i]);
    }

    for (size_t i = 0; i < m_mesh.textures.size(); i++) {
        std::cout << "texture " << i << " " << m_mesh.textures[i] << std::endl;
    }
    m_geometry->dirtyDisplayList();
}
