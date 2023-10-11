#pragma once
#include <osgGA/GUIEventHandler>

class KeyHandler : public osgGA::GUIEventHandler
{
private:
public:
    KeyHandler() = default;
    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
    ~KeyHandler() = default;
    bool isStop   = false;
};
