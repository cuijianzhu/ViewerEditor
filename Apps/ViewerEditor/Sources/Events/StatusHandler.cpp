#include "StatusHandler.h"
#include "Layers/SelectingLayer.h"
#include "Mesh/Mesh.h"
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
StatusHandler::StatusHandler(osg::ref_ptr<SelectingLayer> selectingLayer)
    : m_selectingLayer(selectingLayer)
{
    m_geode = new osg::Geode;
}

bool StatusHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (isSelecting && isPressing) {
        m_selectingLayer->updateGeometry();
        m_geode->removeChild(0, m_geode->getNumChildren());
        auto&              mesh   = m_selectingLayer->m_mesh->m_mesh;
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        auto               root   = viewer->getSceneData()->asGroup();
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

                    float                            radius = 1.0;
                    osg::Vec3f point = intersection.getLocalIntersectPoint();
                    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(
                        new osg::Sphere(intersection.getWorldIntersectPoint(), radius));
                    m_geode->addDrawable(shape.get());
                    root->addChild(m_geode);
                    m_selectingLayer->m_mesh->pickSphere(point, radius, isInvertSelection);
                    std::cout << "Intersection point: " << point.x() << ", "
                                             << point.y() << ", " << point.z() << std::endl;
                }
            }
        }

        return true;
    }

    return false;
}