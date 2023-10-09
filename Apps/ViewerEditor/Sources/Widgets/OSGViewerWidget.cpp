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

#include "GlobalSignal.h"
#include "Mesh/Mesh.h"
OSGViewerWidget::OSGViewerWidget(QWidget* parent)
    : osgQOpenGLWidget(parent)
{
    setMouseTracking(true);
    initConnect();
}

OSGViewerWidget::~OSGViewerWidget()
{}

void OSGViewerWidget::slot_import(const QString& path_) {
    std::string path = path_.toLocal8Bit().constData();
    m_mesh->read(path);
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

void OSGViewerWidget::resizeEvent(QResizeEvent* event)
{
    if (m_floatTools == NULL) {
        m_floatTools = new FloatTools(this);
    }
    QOpenGLWidget::resizeEvent(event);
}

void OSGViewerWidget::initConnect() {
    connect(this, &osgQOpenGLWidget::initialized, this, &OSGViewerWidget::init);
    connect(&g_globalSignal, &GLobalSignal::signal_importMesh, this, &OSGViewerWidget::slot_import);
    connect(&g_globalSignal, &GLobalSignal::signal_exportMesh, this, &OSGViewerWidget::slot_export);

    connect(&g_globalSignal, &GLobalSignal::signal_viewHome, [&]() { 
        getOsgViewer()->home();
    });
}



void OSGViewerWidget::init()
{
    m_root = new osg::Group;
    m_mesh              = new Mesh;
    m_root->addChild(m_mesh);
    m_cameraManipulator = new osgGA::MultiTouchTrackballManipulator();
    getOsgViewer()->setCameraManipulator(m_cameraManipulator);
    getOsgViewer()->setSceneData(m_root);

    osg::Camera* camera = getOsgViewer()->getCamera();
    camera->setClearColor(
        osg::Vec4(0.9529411764705882, 0.9529411764705882, 0.9529411764705882, 1.0));


}
