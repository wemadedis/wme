#include "rte/MeshComponent.h"

#include "rte/RenderingManager.h"

namespace RTE::StdComponents
{

MeshComponent::MeshComponent()
{
}

void MeshComponent::Initialize(std::string meshFilePath, std::string textureFilePath)
{
    _renderingManager = RenderingManager::GetInstance();
    _meshPath = meshFilePath;
    _texturePath = textureFilePath;
    _renderingManager->RegisterMeshComponent(this);
}

void MeshComponent::SetMesh(std::string meshFilePath)
{
    _meshPath = meshFilePath;

}

void MeshComponent::SetTexture(std::string textureFilePath)
{
    _texturePath = textureFilePath;
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