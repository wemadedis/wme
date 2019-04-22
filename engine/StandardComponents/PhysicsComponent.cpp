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

Physics::RigidBody *PhysicsComponent::GetRigidBody()
{
    return _rigidBody;
}
} // namespace RTE::StandardComponents
