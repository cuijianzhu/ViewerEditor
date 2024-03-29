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
    void               updateDashWireGeometry();
    void               pushBackDashWire(osg::Vec3 position_);
    void               clearDashWire();

    void               linkSelection();
    void               clearSelection();
    void               clearSelectionRender();
    void               showBorder();
    void               fillHole();
    void               flat();
    std::string        holeTextPath();
    void               setupMesh(osg::ref_ptr<Mesh>);
    std::string               editTexture();

    void               hide();
    void               show();
    osg::ref_ptr<Mesh> m_mesh;
    osg::Matrixd       m_vpmMatrix;

private:
    void initFilled();
    void initWireFrame();
    void initDashWireFrame();
    void initGeometry();

private:
    osg::ref_ptr<osg::Geode>      m_filled;
    osg::ref_ptr<osg::Geode>      m_wireframe;
    osg::ref_ptr<osg::Geode>      m_dashWireframe;
    osg::ref_ptr<osg::Geometry>   m_geometry;
    osg::ref_ptr<osg::DrawArrays> m_drawArray;
    osg::ref_ptr<osg::Vec3Array>  m_vec3Array;


    osg::ref_ptr<osg::Geometry>   m_dashWireGeometry;
    osg::ref_ptr<osg::DrawArrays> m_dashWireDrawArray;
    osg::ref_ptr<osg::Vec3Array>  m_dashWireVec3Array;

    int                           m_holeNo = 0;
};