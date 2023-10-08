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

class OSGViewerWidget : public osgQOpenGLWidget
{
    Q_OBJECT

public:
    explicit OSGViewerWidget(QWidget* parent = nullptr);
    ~OSGViewerWidget();

public slots:
    void init();

private:
    osg::ref_ptr<osgGA::CameraManipulator> m_cameraManipulator;
    osg::ref_ptr<osg::Group> m_root;
};

#endif   // OSGVIEWERWIDGET_H
