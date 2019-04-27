#include "rte/DirectionalLightComponent.hpp"
#include "rte/RenderingManager.h"

namespace RTE::StdComponents
{

DirectionalLightComponent::~DirectionalLightComponent()
{

}

void DirectionalLightComponent::Initialize(TransformComponent *transform, Colors::Color color)
{
    _renderingManager = RenderingManager::GetInstance();
    _transform = transform;
    _renderingManager->RegisterDirectionalLight(this);
    SetEnabled(true);
}

glm::vec3 DirectionalLightComponent::Direction()
{
    using namespace glm;
    vec3 rot = _transform->Transform.Rot;
    float x = radians(rot.x);
    float y = radians(rot.y);
    return normalize(vec3(eulerAngleYX(y,x) * vec4(0.0f, 0.0f, 1.0f, 0.0f)));
}

void DirectionalLightComponent::Update(float deltaTime) 
{

}

}