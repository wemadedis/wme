#pragma once
#include "rte/LineDebugDrawModule.h"

#include <Bullet/btBulletDynamicsCommon.h>

namespace RTE::Physics
{

class PhysicsDebugDraw : public btIDebugDraw, public Rendering::LineDebugDrawModule
{
    DebugDrawModes mode = (DebugDrawModes)(DBG_DrawWireframe | DBG_DrawContactPoints);
    btDiscreteDynamicsWorld *_world;
public:

    PhysicsDebugDraw(btDiscreteDynamicsWorld *world)
    {
        _world = world;
    }

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        Rendering::Line l;
        l.Start.Position = {from.x(), from.y(), from.z()};
        l.End.Position = {to.x(), to.y(), to.z()};
        RecordLine(l);
    }

    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
                          const btVector3 &color)
    {
        Rendering::Line l;
        l.Start.Position = { PointOnB.x(), PointOnB.y(), PointOnB.z()};
        l.End.Position = l.Start.Position + glm::vec3(normalOnB.x(), normalOnB.y(), normalOnB.z())*3.0f;
        RecordLine(l);
    }

    void reportErrorWarning(const char *warningString) 
    {
        std::cout << warningString << std::endl;
    }


    void draw3dText(const btVector3 &location, const char *textString)
    {

    }

    void setDebugMode(int debugMode)
    {
        mode = static_cast<DebugDrawModes>(debugMode);
    }

    int getDebugMode() const 
    {
        return mode;
    }

    void RecordLines()
    {
        _world->debugDrawWorld();
    }
    
};

}