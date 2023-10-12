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

};

extern GLobalSignal g_globalSignal;