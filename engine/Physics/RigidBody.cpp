#include "rte/RigidBody.hpp"

#include "rte/GlmWrapper.hpp"

#include "rte/PhysicsManager.hpp"
#include "rte/RenderStructs.h"

namespace RTE::Physics
{
RigidBody::RigidBody(btRigidBody *body, Rendering::Transform trans)
{
    _rigidBody = body;

    _oldTransform = btTransform(Convert(trans.RotationMatrix()), Convert(trans.Pos));
}

void RigidBody::SetLinearVelocity(glm::vec3 vel)
{
    _rigidBody->setLinearVelocity(Convert(vel));
}

glm::vec3 RigidBody::GetLinearVelocity()
{
    return Convert(_rigidBody->getLinearVelocity());
}

bool RigidBody::GetKinematic()
{
    return _rigidBody->isKinematicObject();
}

void RigidBody::SetKinematic(bool isKinematic)
{
    int32_t colllisionFlags = _rigidBody->getCollisionFlags();
    if (isKinematic)
    {
        colllisionFlags |= _rigidBody->CF_KINEMATIC_OBJECT;
    }
    else
    {
        colllisionFlags &= ~(1UL << _rigidBody->CF_KINEMATIC_OBJECT);
    }
    _rigidBody->setCollisionFlags(colllisionFlags);
}

bool RigidBody::GetStatic()
{
    return _rigidBody->isStaticObject();
}

void RigidBody::SetStatic(bool isStatic)
{
    int32_t colllisionFlags = _rigidBody->getCollisionFlags();
    if (isStatic)
    {
        colllisionFlags |= _rigidBody->CF_STATIC_OBJECT;
    }
    else
    {
        colllisionFlags &= ~(1UL << _rigidBody->CF_STATIC_OBJECT);
    }
    _rigidBody->setCollisionFlags(colllisionFlags);
}

bool RigidBody::GetStaticOrKinematic()
{
    return _rigidBody->isStaticOrKinematicObject();
}

glm::vec3 RigidBody::GetAngularFactor()
{
    return Convert(_rigidBody->getAngularFactor());
}

void RigidBody::SetLinearFactor(glm::vec3 fac)
{
    _rigidBody->setLinearFactor(Convert(fac));
}

glm::vec3 RigidBody::GetLinearFactor()
{
    return Convert(_rigidBody->getLinearFactor());
}

void RigidBody::SetAngularFactor(glm::vec3 fac)
{
    _rigidBody->setAngularFactor(Convert(fac));
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
    // get phys transform
    btTransform physTransform;
    _rigidBody->getMotionState()->getWorldTransform(physTransform);

    // btVector3 dPos = physTransform.getOrigin() - _oldTransform.getOrigin();
    // btQuaternion dRot = physTransform.getRotation() - _oldTransform.getRotation();

    transform.Pos = Convert(physTransform.getOrigin());
    transform.Rot = glm::radians(glm::eulerAngles(Convert(physTransform.getRotation())));

    // Apply physics change to real transform
    //transform.Pos += Convert(dPos);
    //transform.Rot += glm::eulerAngles(Convert(dRot));

    // Override physics change with real world change
    //UpdateToPhysicsWorld(transform.Pos, transform.Rot);

    _rigidBody->getMotionState()->getWorldTransform(physTransform);
    _oldTransform = physTransform;
}

void RigidBody::UpdateToPhysicsWorld(glm::vec3 position, glm::vec3 orientation)
{
    btTransform initialTransform;

    initialTransform.setOrigin(Convert(position));

    glm::quat rotation(glm::eulerAngleXYZ(orientation.x,
                                          orientation.y,
                                          orientation.z));
    initialTransform.setRotation(Convert(rotation));

    _rigidBody->setWorldTransform(initialTransform);
    _rigidBody->getMotionState()->setWorldTransform(initialTransform);
}
} // namespace RTE::Physics