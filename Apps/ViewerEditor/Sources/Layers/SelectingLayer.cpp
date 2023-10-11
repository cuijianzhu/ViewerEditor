#include "SelectingLayer.h"
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/LineWidth>
#include <osgDB/ReadFile>
SelectingLayer::SelectingLayer() {
    m_filled = new osg::Group;
    m_wireframe = new osg::Group;
    initFilled();
    initWireFrame();
    addChild(m_filled);
    addChild(m_wireframe);
}

void SelectingLayer::initFilled() {
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
    float          alpha    = 0.1f;
    material->setColorMode(osg::Material::OFF);
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 0.64f, 0.0f, alpha));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 0.64f, 0.0f, alpha));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 0.64f, 0.0f, alpha));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 0.64f, 0.0f, alpha));
    stateset->setAttributeAndModes(material,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    stateset->setTextureMode(
        0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    osg::BlendFunc* blend = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateset->setAttributeAndModes(blend, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_filled->setStateSet(stateset);
}

void SelectingLayer::initWireFrame() {
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
}
