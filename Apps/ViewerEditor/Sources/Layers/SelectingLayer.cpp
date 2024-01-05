#include "SelectingLayer.h"
#include <Mesh/Mesh.h>
#include <osg/Array>
#include <osg/BlendFunc>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osgDB/ReadFile>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/hole.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <QString>
#include <QFileSystemWatcher>
#include <osg/LineStipple>


SelectingLayer::SelectingLayer()
{
    m_filled    = new osg::Geode;
    m_wireframe = new osg::Geode;
    m_geometry  = new osg::Geometry;
    m_drawArray = new osg::DrawArrays(GL_TRIANGLES, 0, 0);
    m_vec3Array = new osg::Vec3Array;
    m_dashWireframe = new osg::Geode;

    m_dashWireGeometry = new osg::Geometry;
    m_dashWireDrawArray = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 0);
    m_dashWireVec3Array = new osg::Vec3Array;

    initGeometry();
    initFilled();
    initWireFrame();
    initDashWireFrame();
    addChild(m_filled);
    addChild(m_wireframe);
    addChild(m_dashWireframe);
}

void SelectingLayer::initFilled()
{
    osg::ref_ptr<osg::StateSet>      stateset   = new osg::StateSet;
    osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
    polyoffset->setFactor(-1.0f);
    polyoffset->setUnits(-1.0f);

    osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
    stateset->setAttributeAndModes(polyoffset,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setAttributeAndModes(polymode,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    osg::Material* material = new osg::Material;
    float          alpha    = 0.5f;
    material->setColorMode(osg::Material::OFF);
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.392, 0.784, 0.196, alpha));
    stateset->setAttributeAndModes(material,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    stateset->setTextureMode(
        0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    osg::BlendFunc* blend = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateset->setAttributeAndModes(blend, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_filled->setStateSet(stateset);
    m_filled->addDrawable(m_geometry);
}

void SelectingLayer::initWireFrame()
{
    osg::ref_ptr<osg::StateSet>      stateset   = new osg::StateSet;
    osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
    polyoffset->setFactor(-1.5f);
    polyoffset->setUnits(-1.5f);

    osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    stateset->setAttributeAndModes(polyoffset,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setAttributeAndModes(polymode,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::OFF);
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 0.0f, 1.0f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 0.0f, 1.0f));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 0.0f, 1.0f));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 1.0f, 0.0f, 1.0f));
    stateset->setAttributeAndModes(material,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    stateset->setTextureMode(
        0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    // set line width
    osg::LineWidth* lineWidth = new osg::LineWidth;
    lineWidth->setWidth(2.0);
    stateset->setAttributeAndModes(lineWidth,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_wireframe->setStateSet(stateset);
    m_wireframe->addDrawable(m_geometry);
}

void SelectingLayer::initDashWireFrame() {
    osg::ref_ptr<osg::StateSet>      stateset   = new osg::StateSet;
    osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
    polyoffset->setFactor(-1.5f);
    polyoffset->setUnits(-1.5f);

    osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    stateset->setAttributeAndModes(polyoffset,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setAttributeAndModes(polymode,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::OFF);
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 1.0f, 1.0f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 1.0f, 1.0f));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0f, 1.0f, 1.0f));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 1.0f, 1.0f, 1.0f));
    stateset->setAttributeAndModes(material,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    stateset->setTextureMode(
        0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    osg::ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
    lineStipple->setFactor(2);
    lineStipple->setPattern(0xFF00);
    stateset->setAttributeAndModes(lineStipple.get(), osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(5);
    stateset->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
    osg::ref_ptr<osg::PolygonMode> polygonMode =
        new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    stateset->setAttributeAndModes(polygonMode, osg::StateAttribute::ON);
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    // set line width
    lineWidth->setWidth(5.0);
    stateset->setAttributeAndModes(lineWidth,
                                   osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    m_dashWireframe->setStateSet(stateset);
    m_dashWireframe->addDrawable(m_dashWireGeometry);
}

void SelectingLayer::initGeometry()
{
    m_geometry->setVertexArray(m_vec3Array);
    m_geometry->addPrimitiveSet(m_drawArray);

    m_dashWireGeometry->setVertexArray(m_dashWireVec3Array);
    m_dashWireGeometry->addPrimitiveSet(m_dashWireDrawArray);
}

void SelectingLayer::updateGeometry()
{
    if (!m_mesh) return;
    m_vec3Array->clear();
    for (auto& f : m_mesh->m_mesh.face) {
        if (f.IsS() && !f.IsD()) {
            for (size_t i = 0; i < 3; i++) {
                auto& p = f.V(i)->P();
                m_vec3Array->push_back({p.X(), p.Y(), p.Z()});
            }
        }
    }
    m_drawArray->setCount(m_vec3Array->size());
    m_vec3Array->dirty();
    m_geometry->dirtyGLObjects();
}

void SelectingLayer::updateDashWireGeometry() {
    if (m_dashWireVec3Array->size() < 2) {
        return;
    }
    m_dashWireDrawArray->setCount(m_dashWireVec3Array->size());
    m_dashWireDrawArray->dirty();
    m_dashWireGeometry->dirtyGLObjects();
}

void SelectingLayer::pushBackDashWire(osg::Vec3 position_) {
    m_dashWireVec3Array->push_back(position_);
    updateDashWireGeometry();
}

void SelectingLayer::clearDashWire() {
    m_dashWireVec3Array->clear();
    m_dashWireDrawArray->setCount(0);
    m_dashWireDrawArray->dirty();
    m_dashWireGeometry->dirtyGLObjects();
}

void SelectingLayer::linkSelection() {
    if (!m_mesh) return;
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m_mesh->m_mesh);
    for (auto& f : m_mesh->m_mesh.face) {

        if (f.IsS() && !f.IsD()) {
            for (size_t i = 0; i < f.VN(); i++) {
                auto& ffp = f.FFp(i);
                if (ffp) {
                    ffp->SetS();
                }
            }
        }
    }
    updateGeometry();
}

void SelectingLayer::clearSelection() {
    vcg::tri::UpdateFlags<MyMesh>::FaceClearS(m_mesh->m_mesh);
    vcg::tri::UpdateFlags<MyMesh>::VertexClearS(m_mesh->m_mesh);
    updateGeometry();
}

void SelectingLayer::clearSelectionRender() {
    m_vec3Array->clear();
    m_drawArray->setCount(0);
    m_vec3Array->dirty();
    m_geometry->dirtyGLObjects();
}

void SelectingLayer::showBorder() {
    if (!m_mesh) return;
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m_mesh->m_mesh);
    typedef typename vcg::face::Pos<MyFace> PosType;
    for (auto& f : m_mesh->m_mesh.face) {
        
        if (f.IsS() && !f.IsD()) {
            int nB = 0;
            for (int j = 0; j < 3; ++j) {
                if (vcg::face::IsBorder<MyFace>(f, j))
                {
                    nB++;
                    f.SetS();   
                    PosType ps(&f, j, f.V(j));
                    PosType pi = ps;
                    do {
                        pi.NextB();
                        pi.f->SetS();
                    } while (pi!=ps);
                }
            }
            if (!nB) f.ClearS();
        }
    }
    updateGeometry();
}

void SelectingLayer::fillHole() {
    if (!m_mesh) return;
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m_mesh->m_mesh);
    for (auto& f : m_mesh->m_mesh.face) {
        if (f.IsS() && !f.IsD()) {
            int nB = 0;
            for (int j = 0; j < 3; ++j) {
                if (vcg::face::IsBorder<MyFace>(f, j)) {
                    vcg::tri::Hole<MyMesh>::PosType ps(&f, j, f.V(j));
                    ps.NextB();
                    std::vector<vcg::tri::Hole<MyMesh>::FacePointer*> filledFaces;
                    /*vcg::tri::Hole<MyMesh>::FillHoleEar<vcg::tri::MinimumWeightEar<MyMesh>>(
                        m_mesh->m_mesh, ps, filledFaces);*/
                    std::vector<vcg::tri::Hole<MyMesh>::Info> vinfo;
                    vcg::tri::Hole<MyMesh>::GetInfo(m_mesh->m_mesh, true, vinfo);
                    int ifbegin = m_mesh->m_mesh.face.size();
                    typename std::vector<vcg::tri::Hole<MyMesh>::Info>::iterator ith;
                    int                                  indCb   = 0;
                    int                                  holeCnt = 0;
                    std::vector<vcg::tri::Hole<MyMesh>::FacePointer*>            facePtrToBeUpdated;
                    for (ith = vinfo.begin(); ith != vinfo.end(); ++ith)
                        facePtrToBeUpdated.push_back(&(*ith).p.f);

                    for (ith = vinfo.begin(); ith != vinfo.end(); ++ith) {
                        vcg::tri::Hole<MyMesh>::FillHoleEar<vcg::tri::MinimumWeightEar<MyMesh>>(
                            m_mesh->m_mesh, (*ith).p, facePtrToBeUpdated);
                    }
                    
                    vcg::tri::UpdateFlags<MyMesh>::FaceClearS(m_mesh->m_mesh);
                    
                    for (size_t fi = ifbegin; fi < m_mesh->m_mesh.face.size(); fi++) {
                        auto& fff =  m_mesh->m_mesh.face[fi];
                        fff.SetS();
                    }
                    
                    vcg::tri::RefineOddEven<MyMesh,
                                            vcg::tri::OddPointLoop<MyMesh>,
                                            vcg::tri::EvenPointLoop<MyMesh>>(
                        m_mesh->m_mesh,
                        vcg::tri::OddPointLoop<MyMesh>(m_mesh->m_mesh),
                        vcg::tri::EvenPointLoop<MyMesh>(),
                        0,
                        true);

                    for (size_t fi = ifbegin; fi < m_mesh->m_mesh.face.size(); fi++) {
                        auto& fff = m_mesh->m_mesh.face[fi];
                        fff.SetS();
                        fff.SetV();
                        for (size_t k = 0; k < 3; k++) {
                            m_mesh->m_mesh.face[fi].WT(k).n() = m_mesh->m_originTextureNumber-1;
                            auto uvCoord =
                                osg::Vec3(fff.V(k)->P().X(), fff.V(k)->P().Y(), fff.V(k)->P().Z()) *
                                m_vpmMatrix;
                            uvCoord[0]                        = (uvCoord[0] + 1.0f) * 0.5f;
                            uvCoord[1]                        = (uvCoord[1] + 1.0f) * 0.5f;
                            m_mesh->m_mesh.face[fi].WT(k).u() = uvCoord[0];
                            m_mesh->m_mesh.face[fi].WT(k).v() = uvCoord[1];
                        }
                    }

                    //m_holeNo++;
                    //m_mesh->m_TexNo++;
                    //auto imageText ="Tex" + std::to_string(m_holeNo) + ".png";
                    //m_mesh->m_mesh.textures.push_back(imageText);

                    //// 创建一个文件系统监视器
                    //QFileSystemWatcher* watcher = new QFileSystemWatcher;

                    //// 添加要监视的文件
                    //QString filePath = QString::fromLocal8Bit(holeTextPath().c_str());
                    //watcher->addPath(filePath);

                    //// 连接文件系统监视器的fileChanged信号到槽函数
                    //QObject::connect(watcher,
                    //                 &QFileSystemWatcher::fileChanged,
                    //                 [&](const QString& path) { m_mesh->updateOSGNode();
                    //    });
                    return;
                }
            }
        }
    }
    //m_mesh->updateOSGNode();
}

void SelectingLayer::flat() {
    
    if (!m_mesh) return;
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m_mesh->m_mesh);
    MyFace* faceN = nullptr;
    for (auto& f : m_mesh->m_mesh.face) {
        if (f.IsS() && !f.IsD()) {
            for (size_t i = 0; i < 3; i++) {
                if ((!f.FFp(i)->IsS())&&faceN==nullptr) {
                    faceN = f.FFp(i);
                }
            }
            f.SetD();
        }
    }
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m_mesh->m_mesh);
    vcg::tri::Clean<MyMesh>::RemoveSmallConnectedComponentsSize(m_mesh->m_mesh, 100);

    faceN->SetS();
    clearSelectionRender();
    fillHole();
}


std::string SelectingLayer::holeTextPath()
{
    std::string imagename = "/Tex" + std::to_string(m_holeNo) + ".png";
    auto rnt = m_mesh->m_rootDir + imagename;
    return rnt;
}

void SelectingLayer::setupMesh(osg::ref_ptr<Mesh> mesh_)
{
    m_mesh = mesh_;
}

std::string SelectingLayer::editTexture()
{
    for (auto& f : m_mesh->m_mesh.face) {
        int Tn = f.WT(0).n();
        if (f.IsS()&&Tn>=m_mesh->m_originTextureNumber) {
            std::string rnt = m_mesh->m_mesh.textures[f.WT(0).n()];
            return rnt;
        }
    }
    return std::string();
}

void SelectingLayer::hide() {
    setNodeMask(0x0);
}

void SelectingLayer::show() {
    setNodeMask(0xffffffff);
}
