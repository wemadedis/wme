#include "rte/RigidBody.hpp"

#include "rte/GlmWrapper.hpp"

#include "rte/PhysicsManager.hpp"
#include "rte/RenderStructs.h"

namespace RTE::Physics
{
RigidBody::RigidBody(btRigidBody *body)
{
    _rigidBody = body;
}

void RigidBody::SetLinearVelocity(glm::vec3 vel)
{
    _rigidBody->setLinearVelocity(Convert(vel));
}

void RigidBody::AddForce(glm::vec3 force)
{
    _rigidBody->applyForce(Convert(force), Convert(glm::vec3(0)));
}

void RigidBody::AddTorque(glm::vec3 torque)
{
    _rigidBody->applyTorque(Convert(torque));
}

void RigidBody::SetMass(float mass, glm::vec3 inertia)
{
    _rigidBody->setMassProps(mass, Convert(inertia));
}

void RigidBody::UpdateFromPhysicsWorld(Rendering::Transform &transform)
{
    btTransform physTransform;
    _rigidBody->getMotionState()->getWorldTransform(physTransform);
    glm::vec3 pos = Convert(physTransform.getOrigin());
    transform.Pos = pos;
    btQuaternion rot = physTransform.getRotation();
    glm::highp_f64quat inputQuat(
        rot.w(),
        rot.x(),
        rot.y(),
        rot.z());
    transform.Rot = glm::degrees(glm::eulerAngles(inputQuat));
}

} // namespace RTE::Physics