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

RTEConfig *config;

void RTECore::RunUpdateLoop()
{
    using namespace Platform;
    TimePoint lastFrameEnd = Clock::now();
    float minFrameTime = 1.0f / config->GraphicsConfig.FramesPerSecond;
    float deltaTime = 0.0f;

    // std::vector<float> frameTimes;
    // frameTimes.reserve(frameCount);
    // for (int i = 0; i < frameCount; i++)
    while (_windowManager->ShouldClose() == false)
    {
        for (int32_t moduleIndex = 0;
             moduleIndex < _modules.size();
             moduleIndex++)
        {
            _modules[moduleIndex]->Update(deltaTime);
        }

        float time = std::chrono::duration_cast<FpSeconds>(Clock::now() - lastFrameEnd).count();
        while (time < minFrameTime)
        {
            time = std::chrono::duration_cast<FpSeconds>(Clock::now() - lastFrameEnd).count();
        }
        deltaTime = duration_cast<FpSeconds>(Clock::now() - lastFrameEnd).count();
        // frameTimes.push_back(deltaTime);
        lastFrameEnd = Clock::now();
    }

    // float sum = 0;
    // for (int i = 0; i < frameTimes.size(); i++)
    // {
    // sum += frameTimes[i];
    // }
    // float avg = sum / frameCount;
    // Debug(std::to_string(avg));
}

void RTECore::ValidateConfiguration()
{
    // If the user requests raytracing while it is not available, crash
    if (config->GraphicsConfig.UseRaytracing && config->GraphicsConfig.RaytracingAvailable == false)
    {
        throw RTEException("Tried to use Raytracing when not available");
    }
}

RTECore::RTECore()
{
    config = RTEConfig::GetInstance();
    config->GraphicsConfig.RaytracingAvailable = Rendering::RenderingManager::RayTracingAvailable();

    if (ConfigureGame != nullptr)
    {
        ConfigureGame(config);
    }

    ValidateConfiguration();


    _windowManager = new Platform::WindowManager();
    Rendering::RenderingManager *rm = new Rendering::RenderingManager(*_windowManager);
    _windowManager->SetRenderingManager(rm);
    Runtime::SceneManager *sceneManager = new Runtime::SceneManager();
    

    _modules.push_back(_windowManager);
    _modules.push_back(new Physics::PhysicsManager());
    _modules.push_back(sceneManager);
    _modules.push_back(rm);

    // Call the client initialize function
    if (OnGameStart != nullptr)
    {
        Runtime::Scene *scene = sceneManager->MakeScene();
        sceneManager->SetActiveScene(scene);

        OnGameStart(sceneManager);
    }
    if (config->PhysicsConfig.DebugDrawColliders)
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