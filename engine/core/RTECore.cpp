#pragma once

#include "rte/RTECore.hpp"
#include "rte/RTEException.h"
#include "rte/RTEModule.hpp"
#include "rte/RenderingManager.h"
#include "rte/Utility.hpp"
#include "rte/WindowManager.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace RTE
{
using namespace std::chrono;
typedef high_resolution_clock Clock;
typedef time_point<steady_clock> TimePoint;
using FpSeconds = duration<float, seconds::period>;

void RTECore::InitEngine()
{
    Modules = new std::vector<RTEModule *>();
    Modules->push_back(new Physics::PhysicsManager(Config));
}

void RTECore::RunUpdateLoop()
{
    using namespace Platform;
    TimePoint now = Clock::now();
    float deltaTime = 0.0f;
    while (_windowManager->ShouldClose() == false)
    {
        Platform::PollEvents();
        for (int32_t moduleIndex = 0;
             moduleIndex < Modules->size();
             moduleIndex++)
        {
            Modules->at(moduleIndex)->Update(deltaTime);
        }
        deltaTime = duration_cast<FpSeconds>(Clock::now() - now).count();
        now = Clock::now();
    }
}

void RTECore::ValidateConfiguration()
{
    // todo: (danh) Sun 21/04 - 11:46: Add raytrace check
    bool raytracingAvailable = true;
    Config.GraphicsConfig.UseRaytracing = raytracingAvailable;
    // If the user requests raytracing while it is not available, crash
    if (
        Config.GraphicsConfig.UseRaytracing && raytracingAvailable == false)
    {
        throw RTEException("Tried to use Raytracing when not available");
    }
}

//! Init static field
RTEConfig RTECore::Config;
RTECore::RTECore()
{
    // todo: (danh) Sun 21/04 - 11:46: Add raytrace check
    bool raytracingAvailable = Rendering::RenderingManager::RayTracingAvailable();
    Config.GraphicsConfig.UseRaytracing = raytracingAvailable;

    if (ConfigureGame != nullptr)
    {
        ConfigureGame(Config);
    }

    ValidateConfiguration();
    InitEngine();

    _windowManager = new Platform::WindowManager(Config);
    Rendering::RenderingManager *rm = new Rendering::RenderingManager(Config, *_windowManager);
    _windowManager->SetRenderingManager(rm);
    Modules->push_back(_windowManager);
    Runtime::SceneManager *sceneManager = new Runtime::SceneManager();

    Modules->push_back(sceneManager);
    Modules->push_back(rm);
    
    // Call the client initialize function
    if (OnGameStart != nullptr)
    {
        OnGameStart(*sceneManager);
    }
    rm->FinalizeRenderer();
    // Here user is done setting up scene.
    RunUpdateLoop();
}
} // namespace RTE

//? Do something with command line arguments?
int main(int argc, char const *argv[])
{
    RTE::RTECore core;
    return 0;
}
