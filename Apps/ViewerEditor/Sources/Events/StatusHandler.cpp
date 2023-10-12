#include "StatusHandler.h"
#include "Layers/SelectingLayer.h"
#include "Mesh/Mesh.h"
#include <osgViewer/Viewer>
StatusHandler::StatusHandler(osg::ref_ptr<SelectingLayer> selectingLayer)
    : m_selectingLayer(selectingLayer)
{
    
}

bool StatusHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (isSelecting && isPressing) {
        m_selectingLayer->updateGeometry();
        auto&              mesh   = m_selectingLayer->m_mesh->m_mesh;
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        if (/*ea.getEventType() == osgGA::GUIEventAdapter::RELEASE &&
            ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON*/true) {
            osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
            if (viewer) {
                osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
                    new osgUtil::LineSegmentIntersector(
                        osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

                osgUtil::IntersectionVisitor iv(intersector.get());
                viewer->getCamera()->accept(iv);

                if (intersector->containsIntersections()) {
                    const osgUtil::LineSegmentIntersector::Intersection& intersection =
                        intersector->getFirstIntersection();
                    osg::Vec3f point = intersection.getLocalIntersectPoint();
                    m_selectingLayer->m_mesh->pickSphere(point, 0.5, isInvertSelection);
                    std::cout << "Intersection point: " << point.x() << ", "
                                             << point.y() << ", " << point.z() << std::endl;
                }
            }
        }

        return true;
    }

    return false;
}