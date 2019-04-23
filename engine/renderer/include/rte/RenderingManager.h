#pragma once

#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"
#include "rte/Renderer.h"
#include "rte/WindowManager.h"

namespace RTE::Rendering
{
class RenderingManager : public RTEModule
{
private:
    static RenderingManager *_instance;
    Renderer *_renderer;

public:
    RenderingManager(
        Platform::WindowManager &windowManager,
        RTEConfig &config);
    ~RenderingManager();
    void Update(float deltaTime) override;
};

}; // namespace RTE::Rendering
