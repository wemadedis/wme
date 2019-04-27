#include "rte/PointLightComponent.hpp"
#include "rte/RenderingManager.h"

namespace RTE::StdComponents
{

    TransformComponent* PointLightComponent::GetTransformComponent()
    {
        return _transform;
    }

    void PointLightComponent::Initialize(TransformComponent *transform, Colors::Color color, float radius)
    {
        _renderingManager = RenderingManager::GetInstance();
        _transform = transform;
        Color = color;
        Radius = radius;
        _renderingManager->RegisterPointLight(this);
    }

    void PointLightComponent::Update(float deltaTime)
    {

    }

    PointLightComponent::~PointLightComponent()
    {

    }
};