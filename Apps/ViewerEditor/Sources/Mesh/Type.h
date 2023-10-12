#pragma once
#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/complex.h>
#include <wrap/io_trimesh/export_off.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import.h>
class MyVertex;
class MyEdge;
class MyFace;

struct MyUsedTypes
    : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType, vcg::Use<MyEdge>::AsEdgeType,
                            vcg::Use<MyFace>::AsFaceType>
{};

class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::VFAdj, vcg::vertex::Coord3f,
                         vcg::vertex::Normal3f, vcg::vertex::Color4b, vcg::vertex::Mark,
                         vcg::vertex::TexCoord2f,
                                    vcg::vertex::Qualityf, vcg::vertex::BitFlags>
{
public:
    vcg::math::Quadric<double>& Qd() { return q; }

private:
    vcg::math::Quadric<double> q;
};

class MyEdge : public vcg::Edge<MyUsedTypes>
{};

typedef vcg::tri::BasicVertexPair<MyVertex> VertexPair;

class MyFace : public vcg::Face<MyUsedTypes, vcg::face::VFAdj, vcg::face::FFAdj,
                                vcg::face::Normal3f, vcg::face::VertexRef, vcg::face::WedgeTexCoord2f, vcg::face::BitFlags>
{};

// the main mesh class
class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>, std::vector<MyFace>>
{};


class MyTriEdgeCollapse
    : public vcg::tri::TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse,
                                              vcg::tri::QInfoStandard<MyVertex>>
{
public:
    typedef vcg::tri::TriEdgeCollapseQuadric<MyMesh, VertexPair, MyTriEdgeCollapse,
                                             vcg::tri::QInfoStandard<MyVertex>>
                                         TECQ;
    typedef MyMesh::VertexType::EdgeType EdgeType;
    inline MyTriEdgeCollapse(const VertexPair& p, int i, vcg::BaseParameterClass* pp)
        : TECQ(p, i, pp)
    {}
};