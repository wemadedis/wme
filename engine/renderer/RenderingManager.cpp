#include "rte/RenderingManager.h"
#include "rte/FrameCallbackFunctions.hpp"
#include "rte/WindowManager.h"

namespace RTE::Rendering
{

RenderingManager::RenderingManager(
    RTEConfig &config,
    Platform::WindowManager &windowManager)
{
    RendererInitInfo info;
    info.Width = config.WindowConfig.WindowWidth;
    info.Height = config.WindowConfig.WindowWidth;
    info.extensions = windowManager.GetRequiredExtensions();
    info.RayTracingOn = config.GraphicsConfig.UseRaytracing;
    info.BindingFunc = [&](VkSurfaceKHR &surface, VkInstance instance) {
        windowManager.CreateSurface(instance, surface);
    };

    _renderer = new Renderer(info);
    _renderer->Finalize();
}

RenderingManager::~RenderingManager()
{
}

void RenderingManager::FrameResized(int32_t width, int32_t height)
{
    _renderer->FrameResized(width, height);
}

void RenderingManager::Update(float deltaTime)
{
    _renderer->Draw();
    //! Prolly do something maybe
}

}; // namespace RTE::Rendering