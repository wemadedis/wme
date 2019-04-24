#pragma once

#include <Bullet/btBulletDynamicsCommon.h>
#include <cstdint>

#include "rte/GlmWrapper.hpp"

namespace RTE::StandardComponents
{
class PhysicsComponent;
}
namespace RTE::Physics
{

struct CollisionId
{
    uint32_t CollisionId;
    StandardComponents::PhysicsComponent *Body1;
    StandardComponents::PhysicsComponent *Body2;
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
    int32_t CollisionId;
    uint64_t OtherId;
    glm::vec3 Point;
    bool NewCollision;
};

struct EndCollisionData
{
    int32_t CollisionId;
};

} // namespace RTE::Physics