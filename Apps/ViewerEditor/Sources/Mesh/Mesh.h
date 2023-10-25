#pragma once
#include "Type.h"
#include <osg/Geode>
#include <osg/ref_ptr>
#include <string>
#include <vcg/space/box3.h>
class Mesh : public osg::Group
{
public:
    Mesh();
    void                        read(const std::string& path_);
    void                        write(const std::string& path_);
    void                        pickSphere(osg::Vec3 center_, float raduis_, bool isIvert = false);
    void                        updateOSGNode();
    void                        deleteFace();
    void                        removeSmall();
    osg::ref_ptr<osg::Geometry> m_geometry;
    std::string                 m_rootDir;
    MyMesh                      m_mesh;
    int                         m_TexNo;
};