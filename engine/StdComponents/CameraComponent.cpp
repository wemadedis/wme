#include "rte/CameraComponent.hpp"
#include "rte/RenderingManager.h"



namespace RTE::StdComponents
{


CameraComponent::~CameraComponent()
{

}

void CameraComponent::Initialize(TransformComponent *transformComponent)
{
    _renderingManager = RenderingManager::GetInstance();
    _transform = transformComponent;
}

TransformComponent* CameraComponent::GetTransformComponent()
{
    return _transform;
}

} // namespace RTE::StdComponents


