#include "rte/RigidBody.hpp"

namespace RTE::Physics
{
RigidBody::RigidBody(btRigidBody *body)
{
    _rigidBody = body;
}
} // namespace RTE::Physics