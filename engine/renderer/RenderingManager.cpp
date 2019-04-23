#include "rte/RenderingManager.h"
#include "rte/FrameCallbackFunctions.hpp"

namespace RTE::Rendering
{

RenderingManager::RenderingManager(
    Platform::WindowManager &windowManager,
    RTEConfig &config)
{
    RendererInitInfo info;
    info.Width = config.WindowConfig.WindowWidth;
    info.Height = config.WindowConfig.WindowWidth;
    info.extensions = windowManager.GetRequiredExtensions();
    info.RayTracingOn = config.GraphicsConfig.UseRaytracing;
    info.BindingFunc = [&](VkSurfaceKHR &surface, VkInstance instance) {
        windowManager.CreateSurface(instance, surface);
    };
    info.SetFrameResizeCB = [&](FrameResizeCallback fcb) {
        windowManager.SetFrameResizeCallback(fcb);
    };

    _renderer = new Renderer(info);
    _renderer->Finalize();
}

RenderingManager::~RenderingManager()
{
}

void RenderingManager::Update(float deltaTime)
{
    _renderer->Draw();
    //! Prolly do something maybe
}

}; // namespace RTE::Rendering