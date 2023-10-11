#include "KeyHandler.h"

bool KeyHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (isStop) {
        return true;
    }

    return false;
}