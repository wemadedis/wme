#pragma once

#include <Bullet/btBulletDynamicsCommon.h>
#include <cstdint>

#include "rte/GlmWrapper.hpp"

namespace RTE::StdComponents
{
class PhysicsComponent;
}
namespace RTE::Physics
{

struct CollisionId
{
    uint64_t CollisionId;
    StdComponents::PhysicsComponent *BodyA;
    StdComponents::PhysicsComponent *BodyB;
};

enum class CollisionType
{
    None,
    Started,
    During,
    Ended
};
// uint32_t Flags = 00101
struct OnCollisionData
{
    uint64_t CollisionId;
    uint64_t OtherId;
    glm::vec3 Point;
    bool NewCollision;
};

struct EndCollisionData
{
    int32_t CollisionId;
};

} // namespace RTE::Physics