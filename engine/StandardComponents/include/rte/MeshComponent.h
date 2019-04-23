#pragma once

#include "rte/Component.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"

#include <string>

namespace RTE::StandardComponents
{
using namespace RTE::Rendering;
class MeshComponent : public Runtime::Component
{
    MeshInstanceHandle mesh;

public:
    MeshComponent();
    
    void Initialize(std::string meshFilePath);
    
    void SetTexture(std::string textureFilePath);
    
    virtual void Update(float deltaTime) override;
};

}; // namespace RTE::StandardComponents