#include "OSGViewerWidget.h"
#include "FloatTools.h"

#include <osg/Camera>
#include <osg/Image>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgEarth/Registry>
#include <osgViewer/Viewer>
#include <osgGA/MultiTouchTrackballManipulator>

#include <wrap/io_trimesh/import.h>
#include<vcg/complex/algorithms/hole.h>

#include <QRubberBand>

#include "GlobalSignal.h"
#include "Mesh/Mesh.h"

#include "Events/KeyHandler.h"
OSGViewerWidget::OSGViewerWidget(QWidget* parent)
    : osgQOpenGLWidget(parent)
{
    setMouseTracking(true);
    initConnect();
}

OSGViewerWidget::~OSGViewerWidget()
{}

void OSGViewerWidget::slot_import(const QString& path_) {
    m_root->removeChild(m_mesh);
    m_mesh.release();
    m_mesh           = new Mesh;
    std::string path = path_.toLocal8Bit().constData();
    m_mesh->read(path);
    m_root->addChild(m_mesh);
    getOsgViewer()->home();
}

void OSGViewerWidget::slot_export(const QString& path_) {
    std::string             path = path_.toLocal8Bit().constData();
    osg::ref_ptr<osg::Node> node = getOsgViewer()->getSceneData();

    if (node) {
        osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
        options->setOptionString("OutputTextureFiles");
        osgDB::writeNodeFile(*node, path, options);
    }
}

void OSGViewerWidget::slot_pickFace(bool checked) {
    if (checked) {
        m_keyHandler->isStop = true;
        auto camera          = getOsgViewer()->getCamera();
        auto keyhandler      = new KeyHandler();

        auto vpmMatrix = camera->getViewMatrix() * camera->getProjectionMatrix() *
                         camera->getViewport()->computeWindowMatrix();
        vcg::Point3          p(0, 0, 0);
        vcg::Matrix33<float> vcg_vpmMatrix;
        auto                 aaa = vcg_vpmMatrix * p;
    }
    else {
        m_keyHandler->isStop = false;
    }
    
}

void OSGViewerWidget::resizeEvent(QResizeEvent* event)
{
    if (m_floatTools == NULL) {
        m_floatTools = new FloatTools(this);
    }
    QOpenGLWidget::resizeEvent(event);
}

void OSGViewerWidget::keyPressEvent(QKeyEvent* event) {
    osgQOpenGLWidget::keyPressEvent(event);
}

void OSGViewerWidget::keyReleaseEvent(QKeyEvent* event) {
    osgQOpenGLWidget::keyReleaseEvent(event);
}

void OSGViewerWidget::mousePressEvent(QMouseEvent* event) {
    osgQOpenGLWidget::mousePressEvent(event);
}

void OSGViewerWidget::mouseReleaseEvent(QMouseEvent* event) {
    osgQOpenGLWidget::mouseReleaseEvent(event);
}

void OSGViewerWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    osgQOpenGLWidget::mouseDoubleClickEvent(event);
}

void OSGViewerWidget::mouseMoveEvent(QMouseEvent* event) {
    osgQOpenGLWidget::mouseMoveEvent(event);
}

void OSGViewerWidget::wheelEvent(QWheelEvent* event) {
    osgQOpenGLWidget::wheelEvent(event);
}

void OSGViewerWidget::initConnect() {
    connect(this, &osgQOpenGLWidget::initialized, this, &OSGViewerWidget::init);
    connect(&g_globalSignal, &GLobalSignal::signal_importMesh, this, &OSGViewerWidget::slot_import);
    connect(&g_globalSignal, &GLobalSignal::signal_exportMesh, this, &OSGViewerWidget::slot_export);
    connect(&g_globalSignal, &GLobalSignal::signal_select, this, &OSGViewerWidget::slot_pickFace);

    connect(&g_globalSignal, &GLobalSignal::signal_viewHome, [&]() { 
        getOsgViewer()->home();
    });
}

void OSGViewerWidget::init()
{
    m_root = new osg::Group;
    m_mesh              = new Mesh;
    m_keyHandler = new KeyHandler;
    m_root->addChild(m_mesh);
    m_cameraManipulator = new osgGA::MultiTouchTrackballManipulator();
    auto standardManipulator = (osgGA::StandardManipulator*)m_cameraManipulator.get();
    standardManipulator->setAllowThrow(false);
    getOsgViewer()->setCameraManipulator(m_cameraManipulator);
    getOsgViewer()->setSceneData(m_root);

    osg::Camera* camera = getOsgViewer()->getCamera();
    camera->setClearColor(
        osg::Vec4(0.9529411764705882, 0.9529411764705882, 0.9529411764705882, 1.0));

    getOsgViewer()->addEventHandler(m_keyHandler);
}
