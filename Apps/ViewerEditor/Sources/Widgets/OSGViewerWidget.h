#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <osgQOpenGL/osgQOpenGLWidget>
class OSGViewerWidget : public osgQOpenGLWidget
{
    Q_OBJECT

public:
    explicit OSGViewerWidget(QWidget* parent = nullptr);
    ~OSGViewerWidget();

public slots:
    void init();

private:

};

#endif   // OSGVIEWERWIDGET_H
