#pragma once
#include "rte/Component.h"
#include "rte/StdComponentsMain.hpp"

namespace RTE::Rendering
{
    class RenderingManager;
}

namespace RTE::StdComponents
{

class DirectionalLightComponent : public Runtime::Component
{
private:
    /**
     * @brief Pointer to the rendering manager
     */
    RenderingManager *_renderingManager;

    /**
     * @brief Transform component of the game object that this component is attached to.
     */
    TransformComponent *_transform;
public:
    /**
     * @brief The RGBA color of the light.
     */
    Colors::Color Color = Colors::White;

    /**
     * @brief The default constructor.
     */
    DirectionalLightComponent() = default;
    ~DirectionalLightComponent();

    /**
     * @brief Initializes the point light component.
     * @param transform Transform component attached to the game object.
     * @param color The RGBA color of the light.
     */
    void Initialize(TransformComponent *transform, Colors::Color color);

    /**
     * @brief Gets the transform component.
     * @return Returns the transform component.
     */
    TransformComponent* GetTransformComponent();

    /**
     * @brief Calculates the direction of the light based on the transform component.
     * @return Returns a XYZ direction vector.
     */
    glm::vec3 Direction();

    
    virtual void Update(float deltaTime) override;
};

}