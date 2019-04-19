#pragma once

#include <Bullet/btBulletDynamicsCommon.h>

#include "rte/RTEModule.hpp"
#include "rte/CollisionId.hpp"

namespace RTE::Physics
{

    
class PhysicsManager : public RTEModule
{
private:
    btBroadphaseInterface *broadphase;
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *world;

public:
    PhysicsManager();
    ~PhysicsManager();
};
}; // namespace RTE::Physics