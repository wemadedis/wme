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

void RTECore::RunUpdateLoop()
{
    using namespace Platform;
    TimePoint lastFrameEnd = Clock::now();
    float minFrameTime = 1.0f / Config.GraphicsConfig.FramesPerSecond;
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

        float time = std::chrono::duration_cast<FpSeconds>(Clock::now() - lastFrameEnd).count();
        while (time < minFrameTime)
        {
            time = std::chrono::duration_cast<FpSeconds>(Clock::now() - lastFrameEnd).count();
        }
        deltaTime = duration_cast<FpSeconds>(Clock::now() - lastFrameEnd).count();
        lastFrameEnd = Clock::now();
    }
}

void RTECore::ValidateConfiguration()
{
    // If the user requests raytracing while it is not available, crash
    if (Config.GraphicsConfig.UseRaytracing && Config.GraphicsConfig.RaytracingAvailable == false)
    {
        throw RTEException("Tried to use Raytracing when not available");
    }
}

//! Init static field
RTEConfig RTECore::Config;
RTECore::RTECore()
{
    Config.GraphicsConfig.RaytracingAvailable = Rendering::RenderingManager::RayTracingAvailable();

    if (ConfigureGame != nullptr)
    {
        ConfigureGame(Config);
    }

    ValidateConfiguration();

    Modules = new std::vector<RTEModule *>();

    Modules->push_back(new Physics::PhysicsManager(Config));
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
        Runtime::Scene *scene = sceneManager->MakeScene();
        sceneManager->SetActiveScene(scene);

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
