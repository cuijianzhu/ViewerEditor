#include "StatusHandler.h"
#include "Layers/SelectingLayer.h"
#include "Mesh/Mesh.h"
StatusHandler::StatusHandler(osg::ref_ptr<SelectingLayer> selectingLayer)
    : m_selectingLayer(selectingLayer)
{
    
}

bool StatusHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (isSelecting) {
        m_selectingLayer->updateGeometry();
        auto& mesh = m_selectingLayer->m_mesh->m_mesh;
        return true;
    }

    return false;
}