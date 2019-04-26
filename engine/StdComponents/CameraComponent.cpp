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
    _renderingManager->RegisterCameraComponent(this);
}

TransformComponent* CameraComponent::GetTransformComponent()
{
    return _transform;
}


void CameraComponent::Update(float deltaTime)
{

}

} // namespace RTE::StdComponents


