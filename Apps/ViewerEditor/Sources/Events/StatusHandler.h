#pragma once
#include <osgGA/GUIEventHandler>
#include "Mesh/Type.h"
class SelectingLayer;
namespace osg
{
class MatrixTransform;
}
class StatusHandler : public osgGA::GUIEventHandler
{
public:
    StatusHandler() = default;
    StatusHandler(osg::ref_ptr<SelectingLayer> selectingLayer);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void         removeChild();
    ~StatusHandler()                               = default;
    bool                         isSelecting       = false;
    bool                         isInvertSelection = false;
    bool                         isPressing        = false;
    bool                         isPickAxes        = false;
    bool                         isLassoRegion       = false;
    void                         hide();
    void                         show();
    osg::ref_ptr<osg::Geode>     m_geode;
    osg::ref_ptr<osg::Node>      m_axes;
    osg::ref_ptr<SelectingLayer> m_selectingLayer;
    float                        m_radius = 0.2;
    std::vector<vcg::Point3f>    m_polys;
};
