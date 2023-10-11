#pragma once
#include <osgGA/GUIEventHandler>
class SelectingLayer;
class StatusHandler : public osgGA::GUIEventHandler
{
public:
    StatusHandler() = default;
    StatusHandler(osg::ref_ptr<SelectingLayer> selectingLayer);
    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
    ~StatusHandler() = default;
    bool isSelecting   = false;
    osg::ref_ptr<SelectingLayer> m_selectingLayer;
};