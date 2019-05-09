#pragma once

#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"

//Renderer
#include "rte/Renderer.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"

#include "rte/GUIModule.h"

#include "rte/StdComponentsMain.hpp"
#include "rte/Definitions.hpp"

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

    /**
     * @brief Static pointer to a Rendering Manager instance.
     */
    static RenderingManager *_instance;

    /**
     * @brief Pointer the the renderer instance.
     */
    Renderer *_renderer;

    /**
     * @brief Pointer to a GUI module.
     */
    GUI::GUIModule *_guiModule;

    /**
     * @brief State of ray tracing capabilities, if requested by the user.
     */
    bool _rtEnabled = false;

    /**
     * @brief Mapping from the mesh file paths, to the corresponding mesh handles returned by the renderer
     */
    std::unordered_map<std::string, MeshHandle> _meshes;

    /**
     * @brief Mapping from the texture file paths, to the corresponding texture handles returned by the renderer
     */
    std::unordered_map<std::string, TextureHandle> _textures;
    
    /**
     * @brief Mapping from mesh components to corresponding mesh instances in the renderer.
     */
    std::unordered_map<MeshComponent*, MeshInstanceHandle> _instances;

    /**
     * @brief Mapping from point light components to the corresponding point lights in the renderer.
     */
    std::unordered_map<PointLightComponent*, PointLightHandle> _pointLights;

    /**
     * @brief Mapping from directional light components to the corresponding directional lights in the renderer.
     */
    std::unordered_map<DirectionalLightComponent*, DirectionalLightHandle> _directionalLights;

    /**
     * @brief Mapping of components defining a GUI draw function, to the functions themselves.
     */
    std::unordered_map<Runtime::Component*, GUIDrawFunction> _guiDraws;

    /**
     * @brief Mapping from camera components to the corresponding cameras in the renderer.
     */
    std::vector<CameraComponent*> _cameras;

    /**
     * @brief Current main camera.
     */
    CameraComponent* _mainCamera = nullptr;

    /**
     * @brief Synchronizes the main camera data with the renderer.
     */
    void UpdateMainCamera();

    /**
     * @brief Synchronizes the renderer mesh instance with the mesh component data.
     * @param meshComponent The mesh component.
     */
    void UpdateMeshComponent(MeshComponent *meshComponent);

public:

    /**
     * @brief The constructor of the rendering manager.
     * @param config The configuration for the engine initialization, filled out by the user.
     * @param windowManager The window manager initialized before this class.
     */
    RenderingManager(
        RTEConfig &config,
        Platform::WindowManager &windowManager);
    ~RenderingManager();

    /**
     * @brief Checks whether ray tracing is supported by the GPU device.
     * @return false Ray tracing is not supported.
     * @return true Ray tracing is supported.
     */
    static bool RayTracingAvailable();

    /**
     * @brief Informs the renderer that all necessary data has been uploaded.
     */
    void FinalizeRenderer();

    /**
     * @brief Sets the line debug draw module for the renderer.
     * @param module Pointer to the line debug draw module.
     */
    void SetLineDebugModule(LineDebugDrawModule *module);

    /**
     * @brief Gets the instance of the rendering manager. Used by various runtime components to communicate with the manager.
     * @return Returns an instance of the manager, if it has been created previously. Otherwise returns a null pointer.
     */
    static RenderingManager* GetInstance();

    void Update(float deltaTime) override;

    /**
     * @brief Sets the rendering mode of the renderer.
     * @param rtEnabled If true, sets the rendering mode of the renderer to ray tracing (if it is supported and was requested previously).
     */
    void SetRTEnabled(bool rtEnabled);

    /**
     * @brief Function used by the window manager as callback when the window is resized.
     * @param width The new width of the window.
     * @param height The new height of the window.
     */
    void FrameResized(int32_t width, int32_t height);

    /**
     * @brief Imports all meshes and texture images specified by their absolute file paths. 
     * @param meshes The vector of absolute file paths of the mesh data files.
     * @param textures The vector of absolute file paths of the texture image files.
     */
    void ImportRenderingResources(std::vector<std::string> &meshes, std::vector<std::string> &textures);

    /**
     * @brief Registers a mesh component and creates a corresponding mesh instance with the renderer. 
     * @param meshComponent The mesh component.
     */
    void RegisterMeshComponent(MeshComponent *meshComponent);

    /**
     * @brief Registers a camera component. Used as the main camera if no other component was registered.
     * @param cameraComponent The camera component.
     */ 
    void RegisterCameraComponent(CameraComponent *cameraComponent);

    /**
     * @brief Sets a camera component as the main camera to be used by the renderer. 
     * @param cameraComponent The camera component.
     */
    void SetMainCamera(CameraComponent *cameraComponent);

    /**
     * @brief Registers a point light component and adds a corresponding point light to the renderer. 
     * @param pointLight The point light component.
     */
    void RegisterPointLight(PointLightComponent *pointLight);

    /**
     * @brief Registers a directional light component and adds a corresponding directional light to the renderer. 
     * @param dirLight The directional light component.
     */
    void RegisterDirectionalLight(DirectionalLightComponent *dirLight);

    /**
     * @brief Registers a GUI draw function from a runtime component to pass it to the GUI module.
     * @param comp The runtime component specifying the draw function.
     * @param func The GUI draw function.
     */
    void RegisterGUIDrawFunction(Runtime::Component* comp, GUIDrawFunction func);

    /**
     * @brief Gets the current frame size. Used by camera components in projection matrix calculation.
     * @return Returns a two component vector specifying the width and the height of the renderer frame size.
     */
    glm::ivec2 GetRendererFrameSize();
};

}; // namespace RTE::Rendering
