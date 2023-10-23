#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <QPoint>
#include <osg/ref_ptr>
#include <osgQOpenGL/osgQOpenGLWidget>
namespace osgGA {
class CameraManipulator;
}

namespace osg {
class Group;
}

class FloatTools;
class Mesh;
class StatusHandler;
class QRubberBand;
class SelectingLayer;

class OSGViewerWidget : public osgQOpenGLWidget
{
    Q_OBJECT

public:
    explicit OSGViewerWidget(QWidget* parent = nullptr);
    ~OSGViewerWidget();

public slots:
    void init();
    void slot_import(const QString& path_);
    void slot_export(const QString& path_);
    void slot_pickFace(bool);
    void slot_invertpickFace(bool);
    void slot_deleteFace();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

private:
    void initConnect();

private:
    osg::ref_ptr<osgGA::CameraManipulator> m_cameraManipulator;
    osg::ref_ptr<osg::Group>               m_root;
    osg::ref_ptr<Mesh>                     m_mesh;
    osg::ref_ptr<SelectingLayer>           m_selectingLayer;
    osg::ref_ptr<StatusHandler>            m_statusHandler;
    FloatTools*                            m_floatTools;
    QRubberBand*                           m_rubberBand;
    QPoint                                 m_origin;
    bool                                   m_isPress = false;
};

#endif   // OSGVIEWERWIDGET_H
