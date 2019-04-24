#pragma once

#include "rte/Component.h"
#include "rte/GlmWrapper.hpp"
#include "rte/RenderStructs.h"

namespace RTE::StdComponents
{

/**
 * @brief Holds transform information for use in other components
 * 
 */
class TransformComponent : public Runtime::Component
{
public:
    /**
     * @brief The position of this GameObject
     * 
     */
    Rendering::Transform Transform;

    /**
     * @brief Initialize this TransformComponent using an existing Transform
     * 
     * @param trans Transform to use
     */
    void Initialize(Rendering::Transform trans);

    /**
     * @brief Initialize 
     * 
     * @param pos Position 
     * @param rot Rotation
     * @param scale Scale
     */
    void Initialize(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

    /**
     * @brief Does nothing
     * 
     * @param deltaTime Time since last frame, not used.
     */
    void Update(float deltaTime) override;
};
} // namespace RTE::StdComponents
