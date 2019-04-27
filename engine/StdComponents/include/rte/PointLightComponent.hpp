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
    RenderingManager *_renderingManager;
    TransformComponent *_transform;
public:
    Colors::Color Color = Colors::White;
    float Radius = 10.0f;
    PointLightComponent() = default;
    ~PointLightComponent();
    TransformComponent* GetTransformComponent();
    void Initialize(TransformComponent *transform, Colors::Color color, float radius);
    virtual void Update(float deltaTime) override;
};

};