#include "StatusHandler.h"
#include "Layers/SelectingLayer.h"
StatusHandler::StatusHandler(osg::ref_ptr<SelectingLayer> selectingLayer)
    : m_selectingLayer(selectingLayer)
{
    
}

bool StatusHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (isSelecting) {
        m_selectingLayer->updateGeometry();
        return true;
    }

    return false;
}