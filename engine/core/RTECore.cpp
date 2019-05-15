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
#include <thread>
#include <vector>

namespace RTE
{
using Time = std::chrono::steady_clock;
using double_sec = std::chrono::duration<double>;
using double_time_point = std::chrono::time_point<Time, double_sec>;

void RTECore::RunUpdateLoop()
{
    using namespace Platform;
    double_time_point lastFrameEnd = Time::now();

    double_sec minFrameTime = double_sec(1.0 / Config.GraphicsConfig.FramesPerSecond);
    float deltaTime = 0.0f;

    while (_windowManager->ShouldClose() == false)
    {
        for (int32_t moduleIndex = 0;
             moduleIndex < Modules->size();
             moduleIndex++)
        {
            Modules->at(moduleIndex)->Update(deltaTime);
        }

        double_sec timePassed = double_sec(Time::now() - lastFrameEnd);
        auto timeToSleep = minFrameTime - timePassed;
        if (timePassed < minFrameTime)
        {
            std::this_thread::sleep_for(timeToSleep);
        }

        timePassed = double_sec(Time::now() - lastFrameEnd);
        deltaTime = static_cast<float>(double_sec(Time::now() - lastFrameEnd).count());
        Debug(std::to_string(deltaTime));
        lastFrameEnd = Time::now();
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

    _windowManager = new Platform::WindowManager(Config);
    Modules->push_back(_windowManager);
    Modules->push_back(new Physics::PhysicsManager(Config));
    Rendering::RenderingManager *rm = new Rendering::RenderingManager(Config, *_windowManager);
    _windowManager->SetRenderingManager(rm);
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
    if (Config.PhysicsConfig.DebugDrawColliders)
    {
        rm->SetLineDebugModule(Physics::PhysicsManager::GetInstance()->GetPhysicsDebugDraw());
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
