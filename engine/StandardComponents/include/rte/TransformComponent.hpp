#pragma once

#include "rte/Component.h"
#include "rte/GlmWrapper.hpp"
#include "rte/RenderStructs.h"

namespace RTE::StandardComponents
{

class TransformComponent : public Runtime::Component
{
public:
    Rendering::Transform Transform;
    void Initialize(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
    void Initialize(Rendering::Transform);
    void Update(float deltaTime) override;
};
} // namespace RTE::StandardComponents
