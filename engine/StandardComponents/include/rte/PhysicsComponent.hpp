#pragma once

#include "rte/Component.h"
#include "rte/PhysicsManager.hpp"
#include "rte/RigidBody.hpp"
#include "rte/TransformComponent.hpp"

namespace RTE::StandardComponents
{
class PhysicsComponent : public Runtime::Component
{
private:
    TransformComponent *_transformComponent;
    Physics::RigidBody *_rigidBody;

public:
    void Initialize(TransformComponent *transformComp);
    void Update(float deltaTime) override;
    void SetLinearVelocity(glm::vec3 vel);
    void AddForce(glm::vec3 force);
    void AddTorque(glm::vec3 torque);
    void SetMass(float mass, glm::vec3 inertia);
    void UpdateFromPhysicsWorld(Rendering::Transform &transform);
};
} // namespace RTE::StandardComponents
