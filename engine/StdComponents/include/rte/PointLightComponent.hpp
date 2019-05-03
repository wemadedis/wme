#pragma once
#include "rte/Component.h"
#include "rte/TransformComponent.hpp"
#include "rte/Colors.hpp"

namespace RTE::Rendering
{
    class RenderingManager;
}

namespace RTE::StdComponents
{
using namespace RTE::Rendering;
class PointLightComponent : public Runtime::Component
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
     * @brief The radius of the light.
     */
    float Radius = 10.0f;

    /**
     * @brief The default constructor.
     */
    PointLightComponent() = default;
    ~PointLightComponent();

    /**
     * @brief Gets the transform component.
     * @return Returns the transform component.
     */
    TransformComponent* GetTransformComponent();

    /**
     * @brief Initializes the point light component.
     * @param transform Transform component attached to the game object.
     * @param color The RGBA color of the light.
     * @param radius The radius of the light.
     */
    void Initialize(TransformComponent *transform, Colors::Color color, float radius);

    
    virtual void Update(float deltaTime) override;
};

};