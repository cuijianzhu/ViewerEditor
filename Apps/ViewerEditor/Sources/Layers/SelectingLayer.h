#pragma once
#include <osg/Group>
#include <osg/Matrixd>
namespace osg {
class Geometry;
class DrawArrays;
}   // namespace osg

class Mesh;
class SelectingLayer : public osg::Group
{
public:
    SelectingLayer();
    ~SelectingLayer() = default;
    void               updateGeometry();
    void               linkSelection();
    void               clearSelection();
    void               clearSelectionRender();
    void               showBorder();
    void               fillHole();
    void               flat();
    std::string        holeTextPath();
    void               setupMesh(osg::ref_ptr<Mesh>);
    osg::ref_ptr<Mesh> m_mesh;
    osg::Matrixd       m_vpmMatrix;

private:
    void initFilled();
    void initWireFrame();
    void initGeometry();

private:
    osg::ref_ptr<osg::Geode>      m_filled;
    osg::ref_ptr<osg::Geode>      m_wireframe;
    osg::ref_ptr<osg::Geometry>   m_geometry;
    osg::ref_ptr<osg::DrawArrays> m_drawArray;
    osg::ref_ptr<osg::Vec3Array>  m_vec3Array;
    int                           m_holeNo = 0;
};