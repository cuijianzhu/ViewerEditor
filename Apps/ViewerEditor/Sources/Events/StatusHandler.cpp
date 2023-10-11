#include "StatusHandler.h"

bool StatusHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (isSelecting) {
        return true;
    }

    return false;
}