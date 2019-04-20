#include "rte/RigidBody.hpp"

namespace RTE
{
namespace Physics
{
RigidBody::RigidBody(btRigidBody *body)
{
    _rigidBody = body;
}
} // namespace Physics
}; // namespace RTE