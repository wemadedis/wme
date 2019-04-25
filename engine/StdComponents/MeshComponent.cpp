#include "rte/MeshComponent.h"

#include "rte/RenderingManager.h"

namespace RTE::StdComponents
{

MeshComponent::MeshComponent()
{
}

void MeshComponent::Initialize(TransformComponent *transform, std::string meshFilePath, std::string textureFilePath)
{
    _renderingManager = RenderingManager::GetInstance();
    _meshPath = meshFilePath;
    _texturePath = textureFilePath;
    _renderingManager->RegisterMeshComponent(this);
    _transform = transform;
    //TODO: Some assertions here?
}

void MeshComponent::SetMesh(std::string meshFilePath)
{
    _meshPath = meshFilePath;

}

void MeshComponent::SetTexture(std::string textureFilePath)
{
    _texturePath = textureFilePath;
}

TransformComponent* MeshComponent::GetTransformComponent()
{
    return _transform;
}

std::string MeshComponent::GetMesh()
{
    return _meshPath;
}

std::string MeshComponent::GetTexture()
{
    return _texturePath;
}

void MeshComponent::Update(float deltaTime)
{
}

}; // namespace RTE::StdComponents