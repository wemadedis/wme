#include "rte/PhysicsComponent.hpp"

namespace RTE::StandardComponents
{

void PhysicsComponent::Initialize(TransformComponent *transformComp)
{
    _transformComponent = transformComp;
    _rigidBody = Physics::PhysicsManager::GetInstance()->CreateRigidBody();
}

void PhysicsComponent::Update(float deltaTime)
{
    _rigidBody->UpdateFromPhysicsWorld(_transformComponent->Transform);
}

void PhysicsComponent::SetLinearVelocity(glm::vec3 vel)
{
    _rigidBody->SetLinearVelocity(vel);
}

void PhysicsComponent::AddForce(glm::vec3 force)
{
    _rigidBody->AddForce(force);
}

void PhysicsComponent::AddTorque(glm::vec3 torque)
{
    _rigidBody->AddTorque(torque);
}

void PhysicsComponent::SetMass(float mass, glm::vec3 inertia)
{
    _rigidBody->SetMass(mass, inertia);
}
}; // namespace RTE::StandardComponents
