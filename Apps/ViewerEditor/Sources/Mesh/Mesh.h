#pragma once
#include "Type.h"
#include <osg/ref_ptr>
#include <osg/Geode>
#include <string>
class Mesh : public osg::Geode
{
public:
    Mesh();
    void read(const std::string& path_);
    void                         updateOSGNode();
    osg::ref_ptr<osg::Geometry>  m_geometry;
    MyMesh m_mesh;
};