#include "OSGViewerWidget.h"
#include <osg/Camera>
#include <osg/Image>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgEarth/Registry>
#include <osgViewer/Viewer>
#include <osgGA/MultiTouchTrackballManipulator>
OSGViewerWidget::OSGViewerWidget(QWidget* parent)
    : osgQOpenGLWidget(parent)
{
    setMouseTracking(true);
    connect(this, &osgQOpenGLWidget::initialized, this, &OSGViewerWidget::init);
}

OSGViewerWidget::~OSGViewerWidget()
{
}



void OSGViewerWidget::init()
{
    m_root = new osg::Group;
    m_cameraManipulator = new osgGA::MultiTouchTrackballManipulator();
    getOsgViewer()->setCameraManipulator(m_cameraManipulator);
    getOsgViewer()->setSceneData(m_root);
}
