#pragma once
#include <QObject>
#include <osg/ref_ptr>
class GLobalSignal : public QObject
{
    Q_OBJECT

public:
    GLobalSignal() = default;
Q_SIGNALS:
    void signal_viewHome();
    void signal_importMesh(const QString& path_);
    void signal_exportMesh(const QString& path_);
    void signal_select(bool);
    void signal_invertSelect(bool);
    void signal_deleteFace();
    void signal_linkFace();
    void signal_clearSelect();
    void signal_showBorder();
    void signal_fillHole();
    void signal_flat();
    void signal_removeSmall();
    void signal_refresh();
    void signal_pickAxes();
};

extern GLobalSignal g_globalSignal;