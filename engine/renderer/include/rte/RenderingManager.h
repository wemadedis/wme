#pragma once

#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"

//Renderer
#include "rte/Renderer.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"

#include "rte/MeshComponent.h"

#include <unordered_map>
#include <string>


namespace RTE::Platform
{
class WindowManager;
}
namespace RTE::Rendering
{
class RenderingManager : public RTEModule
{
private:
    static RenderingManager *_instance;
    Renderer *_renderer;

    // RESOURCE DATA: filepath -> resource handle
    std::unordered_map<std::string, MeshHandle> _meshes;
    std::unordered_map<std::string, TextureHandle> _textures;
    std::unordered_map<std::string, ShaderHandle> _shaders;
    std::unordered_map<StandardComponents::MeshComponent*, MeshInstanceHandle> _instances;

public:
    RenderingManager(
        RTEConfig &config,
        Platform::WindowManager &windowManager);
    ~RenderingManager();
    void Update(float deltaTime) override;
    void FrameResized(int32_t width, int32_t height);
    void ImportRenderingResources(std::vector<std::string> *textures, std::vector<std::string> *meshes);
    void RegisterMeshComponent(StandardComponents::MeshComponent *meshComponent);
    void UpdateMeshComponent(StandardComponents::MeshComponent *meshComponent);
};

}; // namespace RTE::Rendering
