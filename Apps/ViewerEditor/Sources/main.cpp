#include "Widgets/OSGViewerWidget.h"
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    OSGViewerWidget osgViewer;
    osgViewer.show();

    return app.exec();
}