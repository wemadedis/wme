#include "rte/PhysicsComponent.hpp"

#include <sstream>
#include <vector>

#include "rte/Utility.hpp"

namespace RTE::StdComponents
{

void PhysicsComponent::Initialize(
    TransformComponent *transformComp,
    float mass,
    std::vector<Physics::Collider> colliders)
{
    using namespace Physics;
    PhysicsManager *physManager = PhysicsManager::GetInstance();
    _transformComponent = transformComp;

    Collisions = new std::queue<OnCollisionData>();
    EndCollisions = new std::queue<EndCollisionData>();

    _rigidBody = physManager->CreateRigidBody(transformComp->Transform,
                                              mass,
                                              colliders,
                                              this);
    _rigidBody->UpdateToPhysicsWorld(_transformComponent->Transform.Pos, _transformComponent->Transform.Rot);
}

void PhysicsComponent::Update(float deltaTime)
{
    _rigidBody->UpdateFromPhysicsWorld(_transformComponent->Transform);
}

Physics::RigidBody *PhysicsComponent::GetRigidBody()
{
    return _rigidBody;
}
} // namespace RTE::StdComponents
