#include "rte/PhysicsComponent.hpp"
#include "rte/Utility.hpp"
#include <sstream>

namespace RTE::StandardComponents
{

void PhysicsComponent::Initialize(TransformComponent *transformComp)
{
    _transformComponent = transformComp;
    _rigidBody = Physics::
                     PhysicsManager::
                         GetInstance()
                             ->CreateRigidBody(transformComp->Transform);
}

void PhysicsComponent::Update(float deltaTime)
{
    auto pos = _transformComponent->Transform.Pos;
    std::ostringstream ss;
    ss << "Position: "
       << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";

    Debug(ss.str());
    _rigidBody->UpdateFromPhysicsWorld(_transformComponent->Transform);
}

Physics::RigidBody *PhysicsComponent::GetRigidBody()
{
    return _rigidBody;
}
} // namespace RTE::StandardComponents
