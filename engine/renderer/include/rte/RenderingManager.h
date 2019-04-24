#pragma once

#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"
#include "rte/Renderer.h"

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

public:
    RenderingManager(
        RTEConfig &config,
        Platform::WindowManager &windowManager);
    ~RenderingManager();
    void Update(float deltaTime) override;
    void FrameResized(int32_t width, int32_t height);
};

}; // namespace RTE::Rendering
