#include <QApplication>
#include "Widgets/OSGViewerWidget.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    OSGViewerWidget osgViewer;
    osgViewer.show();

    return app.exec();
}