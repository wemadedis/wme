#pragma once

#include "rte/RenderStructs.h"

#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

// Forward declaration
class PhysicsManager;
namespace RTE::Physics
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

    void SetLinearVelocity(glm::vec3 vel);
    void AddForce(glm::vec3 force);
    void AddTorque(glm::vec3 torque);
    void SetMass(float mass, glm::vec3 inertia);
    void UpdateFromPhysicsWorld(Rendering::Transform &transform);
};
} // namespace RTE::Physics