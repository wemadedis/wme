#include "rte/TransformComponent.hpp"

namespace RTE::StandardComponents
{
void TransformComponent::Update(float deltaTime)
{
    //! Transforms should not do anything in update
}

void TransformComponent::Initialize(Rendering::Transform transform)
{
    Transform = transform;
}

void TransformComponent::Initialize(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
    Transform.Pos = pos;
    Transform.Rot = rot;
    Transform.Scale = scale;
}
} // namespace RTE::StandardComponents
