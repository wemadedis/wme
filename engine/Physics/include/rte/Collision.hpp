#pragma once

#include <Bullet/btBulletDynamicsCommon.h>
#include <cstdint>

#include "rte/Definitions.hpp"
#include "rte/GlmWrapper.hpp"

namespace RTE::StdComponents
{
class PhysicsComponent;
} // namespace RTE::StdComponents

namespace RTE::Physics
{

struct Collision
{
    CollisionId Id;
    StdComponents::PhysicsComponent *BodyA;
    StdComponents::PhysicsComponent *BodyB;
};

struct OnCollisionData
{
    CollisionId Id;
    GameObjectId GoId;
    glm::vec3 Point;
    bool NewCollision;
};

struct EndCollisionData
{
    CollisionId Id;
};

} // namespace RTE::Physics