#pragma once
#include <osgGA/GUIEventHandler>

class StatusHandler : public osgGA::GUIEventHandler
{
private:
public:
    StatusHandler() = default;
    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
    ~StatusHandler() = default;
    bool isSelecting   = false;
};
