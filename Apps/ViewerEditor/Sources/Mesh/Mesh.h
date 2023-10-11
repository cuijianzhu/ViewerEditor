#pragma once
#include "Type.h"
#include <osg/ref_ptr>
#include <osg/Geode>
#include <string>
#include <vcg/space/box3.h>
class Mesh : public osg::Geode
{
public:
    Mesh();
    void read(const std::string& path_);
    void rectanglePick(vcg::Box3<MyMesh::ScalarType> box_, osg::Matrix vpmMatrix_, bool isInvertSelection_);
    void                         updateOSGNode();
    void                         deleteFace();
    osg::ref_ptr<osg::Geometry>  m_geometry;
    std::string                  m_rootDir;
    MyMesh m_mesh;
};