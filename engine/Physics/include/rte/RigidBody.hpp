#pragma once

#include "rte/RenderStructs.h"

#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

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
    btTransform _oldTransform;

public:
    /**
     * @brief Construct a new Rigid Body object from a Bullet physics body
     * 
     * @param rigidBody The BulletPhysics rigidbody this RigidBody correlates to 
     *      in the physics world. 
     */
    RigidBody(btRigidBody *rigidBody, Rendering::Transform trans);

    btRigidBody *GetRigidBody();

    glm::vec3 GetLinearVelocity();
    void SetLinearVelocity(glm::vec3 vel);

    void AddForce(glm::vec3 force, glm::vec3 relPos);

    void AddTorque(glm::vec3 torque);

    void SetKinematic(bool kinematic);
    bool GetKinematic();
    void SetStatic(bool isStatic);
    bool GetStatic();
    bool GetStaticOrKinematic();

    glm::vec3 GetAngularFactor();
    void SetAngularFactor(glm::vec3 fac);

    glm::vec3 GetLinearFactor();
    void SetLinearFactor(glm::vec3 fac);

    void SetMass(float mass, glm::vec3 inertia);

    // malloc(sizeof(TransformComponent*) * NumTransforms)
    void UpdateFromPhysicsWorld(Rendering::Transform &trans);
    void UpdateToPhysicsWorld(glm::vec3 position, glm::vec3 orientation);
};
} // namespace RTE::Physics