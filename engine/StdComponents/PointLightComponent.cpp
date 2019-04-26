#include "rte/PointLightComponent.hpp"
#include "rte/RenderingManager.h"

namespace RTE::StdComponents
{

    TransformComponent* PointLightComponent::GetTransformComponent()
    {
        return _transform;
    }

    void PointLightComponent::Initialize(TransformComponent *transform, float radius)
    {
        _renderingManager = RenderingManager::GetInstance();
        _transform = transform;
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