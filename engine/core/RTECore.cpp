#pragma once

#include "rte/RTECore.hpp"
#include "rte/RTEException.h"
#include "rte/RTEModule.hpp"
#include "rte/Utility.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace RTE
{
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
using FpSeconds = std::chrono::duration<float, std::chrono::seconds::period>;

void RTECore::InitEngine(RTEConfig &config)
{
    Modules = new std::vector<RTEModule *>();
    Modules->push_back(new Physics::PhysicsManager(config));
}

void RTECore::RunUpdateLoop()
{
    TimePoint now = Clock::now();
    float deltaTime = 0.0f;
    while (true)
    {
        for (int32_t moduleIndex = 0; moduleIndex < Modules->size(); moduleIndex++)
        {
            Modules->at(moduleIndex)->Update(deltaTime);
        }
        deltaTime = std::chrono::duration_cast<FpSeconds>(Clock::now() - now).count();
        now = Clock::now();
    }
}

RTEConfig RTECore::Config;
RTECore::RTECore()
{
    // todo: (danh) Sun 21/04 - 11:46: Add raytrace check
    bool raytracingAvailable = true;
    Config.GraphicsConfig.UseRaytracing = raytracingAvailable;

    if (ConfigureGame != nullptr)
    {
        ConfigureGame(Config);
    }

    // If the user requests raytracing while it is not available, crash
    if (Config.GraphicsConfig.UseRaytracing == true && raytracingAvailable == false)
    {
        throw RTEException("Tried to use Raytracing when not available");
    }

    InitEngine(Config);

    Runtime::SceneManager *sceneManager = new Runtime::SceneManager();

    Modules->push_back(sceneManager);

    // Call the client initialize function
    if (OnGameStart != nullptr)
    {
        OnGameStart(*sceneManager);
    }

    RunUpdateLoop();
}
} // namespace RTE

//? Do something with command line arguments?
int main(int argc, char const *argv[])
{
    RTE::RTECore core;
    return 0;
}
