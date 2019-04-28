#include "rte/TransformComponent.hpp"
#include "rte/Utility.hpp"

namespace RTE::StdComponents
{

void TransformComponent::Initialize()
{
    Initialize(glm::vec3(0), glm::vec3(0), glm::vec3(1));
}

void TransformComponent::Initialize(Rendering::Transform transform)
{
    Transform = transform;
}

void TransformComponent::Initialize(glm::vec3 pos,
                                    glm::vec3 rot,
                                    glm::vec3 scale)
{
    Transform.Pos = pos;
    Transform.Rot = rot;
    Transform.Scale = scale;
}

void TransformComponent::Update(float deltaTime)
{
    //! Transforms should not do anything in update
}
} // namespace RTE::StdComponents
