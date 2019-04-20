#pragma once

#include "rte/RTE.hpp"

#include <cstdint>

namespace RTE::Physics
{
struct CollisionId
{
    uint32_t collisionId;
    GameObject *rb1;
    GameObject *rb2;
};
}; // namespace RTE::Physics