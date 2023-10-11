#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <osgQOpenGL/osgQOpenGLWidget>
#include <osg/ref_ptr>
namespace osgGA
{
class CameraManipulator;
}

namespace osg
{
class Group;
}

class FloatTools;
class Mesh;
class KeyHandler;

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

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

private:
    void initConnect();

private:
    osg::ref_ptr<osgGA::CameraManipulator> m_cameraManipulator;
    osg::ref_ptr<osg::Group> m_root;
    osg::ref_ptr<Mesh>                     m_mesh;
    osg::ref_ptr<KeyHandler>               m_keyHandler;
    FloatTools*                            m_floatTools;
};

#endif   // OSGVIEWERWIDGET_H
