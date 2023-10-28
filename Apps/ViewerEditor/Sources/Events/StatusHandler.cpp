#include "StatusHandler.h"
#include "Layers/SelectingLayer.h"
#include "Mesh/Mesh.h"
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>
#include "Presets/Axes.h"

static bool pointInPolygon(vcg::Point3f point, std::vector<vcg::Point3f> vs)
{
    /*
     * ray-casting algorithm based on
     * http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
     */

    double x      = point.X();
    double y      = point.Y();
    int    length = vs.size();
    bool   inside = false;
    for (int i = 0, j = length - 1; i < length; j = i++) {
        double xi = vs[j].X(), yi = vs[j].Y();
        double xj = vs[i].X(), yj = vs[i].Y();

        bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
        if (intersect) {
            inside = !inside;
        }
    }
    return (inside);
}

StatusHandler::StatusHandler(osg::ref_ptr<SelectingLayer> selectingLayer)
    : m_selectingLayer(selectingLayer)
{
    m_geode = new osg::Geode;
    m_axes       = presets::Axes();
}

bool StatusHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (isPressing && (isSelecting || isInvertSelection)) {
        removeChild();
        auto&              mesh   = m_selectingLayer->m_mesh->m_mesh;
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        auto               root   = viewer->getSceneData()->asGroup();
        if (/*ea.getEventType() == osgGA::GUIEventAdapter::RELEASE &&
            ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON*/
            true) {
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

                    osg::Vec3f                       point  = intersection.getLocalIntersectPoint();

                    osg::ref_ptr<osg::ShapeDrawable> shape  = new osg::ShapeDrawable(
                        new osg::Sphere(intersection.getWorldIntersectPoint(), m_radius));
                    shape->setColor({1.0, 0.0, 0.0, 0.2});
                    m_geode->addDrawable(shape.get());
                    root->addChild(m_geode);
                    m_selectingLayer->m_mesh->pickSphere(point, m_radius, isInvertSelection);
                }
            }
        }
        return true;
    }

    if (isPickAxes) {
        removeChild();
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        auto               root   = viewer->getSceneData()->asGroup();
        if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE &&
            ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
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
                    viewer->getSceneData()->asGroup()->removeChild(m_axes);
                    m_axes = presets::Axes(intersection.getWorldIntersectPoint());
                    viewer->getSceneData()->asGroup()->addChild(m_axes);
                    osg::Vec3 eye, center, up;
                    viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);
                    viewer->getCameraManipulator()->setHomePosition(
                        eye, intersection.getWorldIntersectPoint(), up);
                }
            }
            isPickAxes = false;
        }
    }

    if (isLassoRegion) {
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        auto               root   = viewer->getSceneData()->asGroup();
        if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE &&
            ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
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
                    vcg::Point3f vcgp = {(float)intersection.getLocalIntersectPoint().x(),
                                         (float)intersection.getLocalIntersectPoint().y(),
                                         (float)intersection.getLocalIntersectPoint().z()};
                    m_polys.push_back(vcgp);
                    m_selectingLayer->pushBackDashWire(intersection.getWorldIntersectPoint());
                }
            }
        }
        if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE &&
            ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
            isLassoRegion = false;
            m_selectingLayer->clearDashWire();
            m_polys.push_back(m_polys[0]);
            auto mesh = m_selectingLayer->m_mesh;
            for (auto& f : mesh->m_mesh.face) {
                if (f.IsD()) continue;
                auto center = vcg::Barycenter(f);
                if (pointInPolygon(center, m_polys)) {
                    continue;
                }
                for (size_t i = 0; i < 3; i++) {
                    if (pointInPolygon(f.V(i)->P(), m_polys)) {
                        break;
                    }
                }
                f.SetD();
            }
            mesh->updateOSGNode();
            m_polys.clear();
        }
    }


    return false;
}

void StatusHandler::removeChild() {
    m_selectingLayer->updateGeometry();
    m_geode->removeChild(0, m_geode->getNumChildren());
}

void StatusHandler::hide() {
    m_geode->setNodeMask(0x0);
    m_axes->setNodeMask(0x0);
}

void StatusHandler::show() {
    m_geode->setNodeMask(0xffffffff);
    m_axes->setNodeMask(0xffffffff);
}
