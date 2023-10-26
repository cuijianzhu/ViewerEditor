#include "Axes.h"
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/PolygonMode>

osg::Node* presets::Axes(osg::Vec3 position, float width)
{
    osg::Group* rnt = new osg::Group;

    // x
    {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        vertices->push_back(position+osg::Vec3(-3.0, 0.0, 0.0));
        vertices->push_back(position + osg::Vec3(3.0, 0.0, 0.0));

        osg::ref_ptr<osg::DrawElementsUInt> indices =
            new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 2);
        indices->push_back(0);
        indices->push_back(1);
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
        for (size_t i = 0; i < 2; i++) {
            colors->push_back(osg::Vec4(1.0, 0.0, 0.0, 1.0f));
        }

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
        geometry->setColorArray(colors.get());
        geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        geometry->setVertexArray(vertices);
        geometry->addPrimitiveSet(indices);
        osg::ref_ptr<osg::StateSet>    stateSet    = geometry->getOrCreateStateSet();
        osg::ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
        lineStipple->setFactor(2);
        lineStipple->setPattern(0xFF00);
        stateSet->setAttributeAndModes(lineStipple.get(), osg::StateAttribute::ON);
        stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(width);
        stateSet->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
        osg::ref_ptr<osg::PolygonMode> polygonMode =
            new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        stateSet->setAttributeAndModes(polygonMode, osg::StateAttribute::ON);
        stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

        rnt->addChild(geometry);
    }

    // y
    {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        vertices->push_back(position + osg::Vec3(0.0, -3.0, 0.0));
        vertices->push_back(position + osg::Vec3(0.0, 3.0, 0.0));

        osg::ref_ptr<osg::DrawElementsUInt> indices =
            new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 2);
        indices->push_back(0);
        indices->push_back(1);
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
        for (size_t i = 0; i < 2; i++) {
            colors->push_back(osg::Vec4(0.0, 1.0, 0.0, 1.0f));
        }

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
        geometry->setColorArray(colors.get());
        geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        geometry->setVertexArray(vertices);
        geometry->addPrimitiveSet(indices);
        osg::ref_ptr<osg::StateSet>    stateSet    = geometry->getOrCreateStateSet();
        osg::ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
        lineStipple->setFactor(2);
        lineStipple->setPattern(0xFF00);
        stateSet->setAttributeAndModes(lineStipple.get(), osg::StateAttribute::ON);
        stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(width);
        stateSet->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
        osg::ref_ptr<osg::PolygonMode> polygonMode =
            new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        stateSet->setAttributeAndModes(polygonMode, osg::StateAttribute::ON);
        stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

        rnt->addChild(geometry);
    }

    // z
    {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        vertices->push_back(position+osg::Vec3(0.0, 0.0, -3.0));
        vertices->push_back(position+osg::Vec3(0.0, 0.0, 3.0));

        osg::ref_ptr<osg::DrawElementsUInt> indices =
            new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 2);
        indices->push_back(0);
        indices->push_back(1);
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
        for (size_t i = 0; i < 2; i++) {
            colors->push_back(osg::Vec4(0.0, 0.0, 1.0, 1.0f));
        }

        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
        geometry->setColorArray(colors.get());
        geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        geometry->setVertexArray(vertices);
        geometry->addPrimitiveSet(indices);
        osg::ref_ptr<osg::StateSet>    stateSet    = geometry->getOrCreateStateSet();
        osg::ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
        lineStipple->setFactor(2);
        lineStipple->setPattern(0xFF00);
        stateSet->setAttributeAndModes(lineStipple.get(), osg::StateAttribute::ON);
        stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(width);
        stateSet->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
        osg::ref_ptr<osg::PolygonMode> polygonMode =
            new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        stateSet->setAttributeAndModes(polygonMode, osg::StateAttribute::ON);
        stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

        rnt->addChild(geometry);
    }

    return rnt;
}