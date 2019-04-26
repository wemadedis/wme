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
    RenderingManager *_renderingManager;
    TransformComponent *_transform;
public:
    Colors::Color Color = Colors::White;


    DirectionalLightComponent() = default;
    ~DirectionalLightComponent();
    void Initialize(TransformComponent *transform, Colors::Color color);
    glm::vec3 Direction();
    virtual void Update(float deltaTime) override;
};

}