#include "OSGViewerWidget.h"
#include "FloatTools.h"

#include <osg/BlendFunc>
#include <osg/Camera>
#include <osg/Image>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgViewer/Viewer>

#include <vcg/complex/algorithms/hole.h>
#include <wrap/io_trimesh/import.h>

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRubberBand>

#include "GlobalSignal.h"
#include "Mesh/Mesh.h"

#include "Events/StatusHandler.h"
#include "Layers/SelectingLayer.h"
#include "Presets/Axes.h"


OSGViewerWidget::OSGViewerWidget(QWidget* parent)
    : osgQOpenGLWidget(parent)
{
    setMouseTracking(true);
    m_floatTools = new FloatTools(this);

    initConnect();
}

OSGViewerWidget::~OSGViewerWidget() {}

void OSGViewerWidget::slot_import(const QString& path_)
{
    m_root->removeChild(0, m_root->getNumChildren());
    m_mesh.release();
    m_mesh           = new Mesh;
    std::string path = path_.toLocal8Bit().constData();
    m_mesh->read(path);
    m_selectingLayer->setupMesh(m_mesh);
    // m_mesh->addChild(m_selectingLayer);
    m_root->addChild(m_mesh);
    m_root->addChild(m_selectingLayer);

    getOsgViewer()->home();
}

void OSGViewerWidget::slot_export(const QString& path_)
{
    m_mesh->write(path_.toLocal8Bit().constData());
}

void OSGViewerWidget::slot_pickFace(bool checked)
{
    if (checked) {
        m_statusHandler->m_radius          = 0.2;
        m_statusHandler->isSelecting       = true;
        m_statusHandler->isInvertSelection = false;
    }
    else {
        m_statusHandler->isSelecting       = false;
        m_statusHandler->isInvertSelection = false;
        m_statusHandler->removeChild();
    }
}

void OSGViewerWidget::slot_invertpickFace(bool checked)
{
    if (checked) {
        m_statusHandler->m_radius = 0.2;
        m_statusHandler->isSelecting       = false;
        m_statusHandler->isInvertSelection = true;
    }
    else {
        m_statusHandler->isSelecting = false;
        m_statusHandler->isInvertSelection = false;
        m_statusHandler->removeChild();
    }
}

void OSGViewerWidget::slot_deleteFace()
{
    m_mesh->deleteFace();
    m_selectingLayer->updateGeometry();
    m_statusHandler->removeChild();
}

void OSGViewerWidget::resizeEvent(QResizeEvent* event)
{
    QOpenGLWidget::resizeEvent(event);
}

void OSGViewerWidget::keyPressEvent(QKeyEvent* event)
{
    osgQOpenGLWidget::keyPressEvent(event);
}

void OSGViewerWidget::keyReleaseEvent(QKeyEvent* event)
{
    osgQOpenGLWidget::keyReleaseEvent(event);
}

void OSGViewerWidget::mousePressEvent(QMouseEvent* event)
{
    osgQOpenGLWidget::mousePressEvent(event);
    m_statusHandler->isPressing = true;
}

void OSGViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    osgQOpenGLWidget::mouseReleaseEvent(event);
    m_statusHandler->isPressing = false;
    m_statusHandler->removeChild();
}

void OSGViewerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    osgQOpenGLWidget::mouseDoubleClickEvent(event);
}

void OSGViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    osgQOpenGLWidget::mouseMoveEvent(event);
}

void OSGViewerWidget::wheelEvent(QWheelEvent* event)
{
    osgQOpenGLWidget::wheelEvent(event);
    if (m_statusHandler->isPressing) {
        if (event->delta() > 0)
            m_statusHandler->m_radius *= 1.1;   // ·Å´ó
        else
            m_statusHandler->m_radius *= 0.9;   // ËõÐ¡
    }
}

void OSGViewerWidget::initConnect()
{
    connect(this, &osgQOpenGLWidget::initialized, this, &OSGViewerWidget::init);
    connect(&g_globalSignal, &GLobalSignal::signal_importMesh, this, &OSGViewerWidget::slot_import);
    connect(&g_globalSignal, &GLobalSignal::signal_exportMesh, this, &OSGViewerWidget::slot_export);
    connect(&g_globalSignal, &GLobalSignal::signal_select, this, &OSGViewerWidget::slot_pickFace);
    connect(&g_globalSignal, &GLobalSignal::signal_linkFace, [&]() {
            m_selectingLayer->linkSelection();
        });
    connect(&g_globalSignal, &GLobalSignal::signal_pickAxes, [&]() {
        m_statusHandler->isPickAxes = true;
    });
    connect(&g_globalSignal, &GLobalSignal::signal_showBorder, [&]() {
        m_selectingLayer->showBorder();
    });
    connect(&g_globalSignal, &GLobalSignal::signal_clearSelect, [&]() {
        m_selectingLayer->clearSelection();
    });

    connect(&g_globalSignal, &GLobalSignal::signal_removeSmall, [&]() { m_mesh->removeSmall();
    });
    connect(&g_globalSignal, &GLobalSignal::signal_refresh, [&]() { m_mesh->updateOSGNode(); });
    connect(&g_globalSignal, &GLobalSignal::signal_lassoRegion, [&]() { m_statusHandler->isLassoRegion = true; });
    connect(&g_globalSignal, &GLobalSignal::signal_withTexture, [&]() { m_mesh->withTexture(); });
    connect(&g_globalSignal, &GLobalSignal::signal_noTexture, [&]() { m_mesh->noTexute(); });
    connect(&g_globalSignal, &GLobalSignal::signal_smooth, [&]() { 
        m_mesh->smooth();
        m_selectingLayer->clearSelection();
        });

    connect(&g_globalSignal, &GLobalSignal::signal_reTexture, [&]() { 
        m_mesh->originRender();
        m_statusHandler->hide();
        m_selectingLayer->hide();
        getOsgViewer()->frame();
        auto image = grabFramebuffer();
        auto camera = getOsgViewer()->getCamera();
        std::string texName = m_mesh->reTexture(camera->getViewMatrix() * camera->getProjectionMatrix());
        QString texPath = QString::fromLocal8Bit("%1/%2")
                              .arg(QString::fromLocal8Bit(m_mesh->m_rootDir.c_str()))
                              .arg(QString::fromLocal8Bit(texName.c_str()));
        std::cout << "texpath: " << texPath.toLocal8Bit().constData() << std::endl;
        image.save(texPath);
        m_statusHandler->show();
        m_selectingLayer->show();
        m_mesh->updateOSGNode();
    });

    connect(&g_globalSignal, &GLobalSignal::signal_fillHole, [&]() {
        auto camera    = getOsgViewer()->getCamera();
        m_selectingLayer->m_vpmMatrix = camera->getViewMatrix() * camera->getProjectionMatrix();
        m_selectingLayer->fillHole();
        m_selectingLayer->clearSelection();
        m_selectingLayer->m_mesh->updateOSGNode();
    });

    connect(&g_globalSignal, &GLobalSignal::signal_flat, [&]() {
        auto camera                   = getOsgViewer()->getCamera();
        m_selectingLayer->m_vpmMatrix = camera->getViewMatrix() * camera->getProjectionMatrix();
        m_statusHandler->removeChild();
        m_selectingLayer->clearSelectionRender();

        m_mesh->m_isOrigin = false;
        m_selectingLayer->flat();
        
        m_selectingLayer->clearSelection();
        m_selectingLayer->m_mesh->updateOSGNode();

        emit g_globalSignal.signal_reTexture();
    });
    connect(&g_globalSignal,
            &GLobalSignal::signal_invertSelect,
            this,
            &OSGViewerWidget::slot_invertpickFace);
    connect(
        &g_globalSignal, &GLobalSignal::signal_deleteFace, this, &OSGViewerWidget::slot_deleteFace);

    connect(&g_globalSignal, &GLobalSignal::signal_viewHome, [&]() { getOsgViewer()->home(); });
}

void OSGViewerWidget::init()
{
    m_root                   = new osg::Group;
    auto            stateSet = m_root->getOrCreateStateSet();
    osg::BlendFunc* blend    = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
    m_mesh           = new Mesh;
    m_selectingLayer = new SelectingLayer;
    m_statusHandler  = new StatusHandler(m_selectingLayer);
    m_root->addChild(m_mesh);
    m_root->addChild(m_selectingLayer);
    //m_root->addChild(osgDB::readNodeFile("D:/data/ply/test1.ply"));

    m_cameraManipulator      = new osgGA::MultiTouchTrackballManipulator();
    auto standardManipulator = (osgGA::StandardManipulator*)m_cameraManipulator.get();
    standardManipulator->setAllowThrow(false);
    getOsgViewer()->setCameraManipulator(m_cameraManipulator);
    getOsgViewer()->setSceneData(m_root);

    osg::Camera* camera = getOsgViewer()->getCamera();
    camera->setClearColor(
        osg::Vec4(0.9529411764705882, 0.9529411764705882, 0.9529411764705882, 1.0));

    getOsgViewer()->addEventHandler(m_statusHandler);
}
