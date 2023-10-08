#pragma once
#include <QObject>
#include <osg/ref_ptr>
#include <osgEarth/GeoTransform>
class GLobalSignal : public QObject
{
    Q_OBJECT

public:
    GLobalSignal() = default;
Q_SIGNALS:
    void signal_viewHome();
};

extern GLobalSignal g_globalSignal;