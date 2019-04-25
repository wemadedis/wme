#pragma once

#include "rte/Component.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"
#include "rte/TransformComponent.hpp"
#include <string>

namespace RTE::Rendering
{
    class RenderingManager;
}

namespace RTE::StdComponents
{
using namespace RTE::Rendering;
class MeshComponent : public Runtime::Component
{
private:
    RenderingManager *_renderingManager;
    std::string _meshPath;
    std::string _texturePath;
    TransformComponent *_transform;
public:
    MeshComponent();

    void Initialize(TransformComponent *transform, std::string meshFilePath, std::string textureFilePath = std::string(""));

    void SetMesh(std::string meshFilePath);

    void SetTexture(std::string textureFilePath);

    std::string GetMesh();

    std::string GetTexture();

    TransformComponent* GetTransformComponent();

    virtual void Update(float deltaTime) override;
};

}; // namespace RTE::StdComponents