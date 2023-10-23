#include "SelectingLayer.h"
#include <Mesh/Mesh.h>
#include <osg/Array>
#include <osg/BlendFunc>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osgDB/ReadFile>

SelectingLayer::SelectingLayer()
{
    m_filled    = new osg::Geode;
    m_wireframe = new osg::Geode;
    m_geometry  = new osg::Geometry;
    m_drawArray = new osg::DrawArrays(GL_TRIANGLES, 0, 0);
    m_vec3Array = new osg::Vec3Array;
    initGeometry();
    initFilled();
    initWireFrame();
    addChild(m_filled);
    addChild(m_wireframe);
}

void SelectingLayer::initFilled()
{
    osg::ref_ptr<osg::StateSet>      stateset   = new osg::StateSet;
    osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
    polyoffset->setFactor(-1.0f);
    polyoffset->setUnits(-1.0f);

    osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
    stateset->setAttributeAndModes(polyoffset,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setAttributeAndModes(polymode,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    osg::Material* material = new osg::Material;
    float          alpha    = 0.5f;
    material->setColorMode(osg::Material::OFF);
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    stateset->setAttributeAndModes(material,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    stateset->setTextureMode(
        0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    osg::BlendFunc* blend = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateset->setAttributeAndModes(blend, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_filled->setStateSet(stateset);
    m_filled->addDrawable(m_geometry);
}

void SelectingLayer::initWireFrame()
{
    osg::ref_ptr<osg::StateSet>      stateset   = new osg::StateSet;
    osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
    polyoffset->setFactor(-1.5f);
    polyoffset->setUnits(-1.5f);

    osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    stateset->setAttributeAndModes(polyoffset,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setAttributeAndModes(polymode,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::OFF);
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 0.0f, 1.0f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 0.0f, 1.0f));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 0.0f, 1.0f));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 1.0f, 0.0f, 1.0f));
    stateset->setAttributeAndModes(material,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    stateset->setTextureMode(
        0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    // set line width
    osg::LineWidth* lineWidth = new osg::LineWidth;
    lineWidth->setWidth(2.0);
    stateset->setAttributeAndModes(lineWidth,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_wireframe->setStateSet(stateset);
    m_wireframe->addDrawable(m_geometry);
}

void SelectingLayer::initGeometry()
{
    m_geometry->setVertexArray(m_vec3Array);
    m_geometry->addPrimitiveSet(m_drawArray);
}

void SelectingLayer::updateGeometry()
{
    if (!m_mesh) return;
    m_vec3Array->clear();
    for (auto& f : m_mesh->m_mesh.face) {
        if (f.IsS() && !f.IsD()) {
            for (size_t i = 0; i < 3; i++) {
                auto& p = f.V(i)->P();
                m_vec3Array->push_back({p.X(), p.Y(), p.Z()});
            }
        }
    }
    m_drawArray->setCount(m_vec3Array->size());
    m_vec3Array->dirty();
    m_geometry->dirtyDisplayList();
}

void SelectingLayer::setupMesh(osg::ref_ptr<Mesh> mesh_)
{
    m_mesh = mesh_;
}
