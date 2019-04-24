#pragma once

#include "rte/Component.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"

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
friend class RenderingManager;
private:
    std::string _meshPath;
    std::string _texturePath;

public:
    MeshComponent();

    void Initialize(std::string meshFilePath);

    void SetTexture(std::string textureFilePath);

    virtual void Update(float deltaTime) override;
};

}; // namespace RTE::StdComponents