#pragma once
#include <osgGA/GUIEventHandler>
class SelectingLayer;
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
    osg::ref_ptr<osg::Geode>     m_geode;
    osg::ref_ptr<SelectingLayer> m_selectingLayer;
    float                        m_radius = 0.2;
};
