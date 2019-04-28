#pragma once

#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"

//Renderer
#include "rte/Renderer.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"

#include "rte/GUIModule.h"

#include "rte/StdComponentsMain.hpp"

#include <unordered_map>
#include <vector>
#include <string>

namespace RTE::Platform
{
class WindowManager;
}
namespace RTE::Rendering
{
using namespace StdComponents;
class RenderingManager : public RTEModule
{
private:
    static RenderingManager *_instance;
    Renderer *_renderer;
    GUI::GUIModule *_guiModule;
    bool _rtEnabled = false;

    // RESOURCE DATA: filepath -> resource handle
    std::unordered_map<std::string, MeshHandle> _meshes;
    std::unordered_map<std::string, TextureHandle> _textures;
    std::unordered_map<std::string, ShaderHandle> _shaders;
    std::unordered_map<MeshComponent*, MeshInstanceHandle> _instances;
    std::unordered_map<PointLightComponent*, PointLightHandle> _pointLights;
    std::unordered_map<DirectionalLightComponent*, DirectionalLightHandle> _directionalLights;
    std::vector<CameraComponent*> _cameras;
    

    CameraComponent* _mainCamera = nullptr;
    void UpdateMainCamera();
    void UpdateMeshComponent(MeshComponent *meshComponent);

public:
    RenderingManager(
        RTEConfig &config,
        Platform::WindowManager &windowManager);
    ~RenderingManager();
    void FinalizeRenderer();
    static RenderingManager* GetInstance();
    void Update(float deltaTime) override;
    void SetRTEnabled(bool rtEnabled);
    void FrameResized(int32_t width, int32_t height);
    void ImportRenderingResources(std::vector<std::string> &meshes, std::vector<std::string> &textures);
    void RegisterMeshComponent(MeshComponent *meshComponent);
    void RegisterCameraComponent(CameraComponent *cameraComponent);
    void SetMainCamera(CameraComponent *cameraComponent);
    void RegisterPointLight(PointLightComponent *pointLight);
    void RegisterDirectionalLight(DirectionalLightComponent *dirLight);
    void SetGUIDrawFunction(void (*function)());
    glm::ivec2 GetRendererFrameSize();
};

}; // namespace RTE::Rendering
