#pragma once

#include <Bullet/btBulletDynamicsCommon.h>

namespace RTE
{
namespace Physics
{
class RigidBody
{
  private:
    /**
     * @brief The Bullet Physics rigid body
     * 
     */
    btRigidBody *_rigidBody;

  public:
    /**
     * @brief Construct a new Rigid Body object from a Bullet physics body
     * 
     * @param rigidBody The BulletPhysics rigidbody this RigidBody correlates to 
     *      in the physics world. 
     */
    RigidBody(btRigidBody *rigidBody);
};
} // namespace Physics
} // namespace RTE