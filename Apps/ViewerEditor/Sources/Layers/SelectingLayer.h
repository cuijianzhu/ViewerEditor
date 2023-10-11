#pragma once
#include <osg/Group>
class SelectingLayer : public osg::Group
{
public:
    SelectingLayer();
    ~SelectingLayer() = default;

private:
    void initFilled();
    void initWireFrame();

private:
    osg::ref_ptr<osg::Group> m_filled;
    osg::ref_ptr<osg::Group> m_wireframe;
};