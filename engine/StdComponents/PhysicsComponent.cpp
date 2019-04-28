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
}

void PhysicsComponent::Update(float deltaTime)
{
    auto pos = _transformComponent->Transform.Pos;
    // TODO: (danh 25/04 14:30): Change this to use transform dirty flag!
    _rigidBody->UpdateFromPhysicsWorld(_transformComponent->Transform, false);
}

Physics::RigidBody *PhysicsComponent::GetRigidBody()
{
    return _rigidBody;
}
} // namespace RTE::StdComponents
