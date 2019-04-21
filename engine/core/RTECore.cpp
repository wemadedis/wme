#pragma once

#include "rte/RTECore.hpp"
#include "rte/RTEException.h"
#include "rte/Scene.h"

#include <cstdint>
#include <vector>

using namespace RTE;

// TODO: (danh 21/04 00:25): Redo this
static Scene scene;
Scene *CreateScene()
{
    return &scene;
}

// todo: (danh) Sun 21/04 - 12:05: This into a struct or something.
static Physics::PhysicsManager *PhysicsManager;
static Runtime::SceneManager *SceneManager;

void RTECore::InitEngine(RTEConfig &config)
{
    //PhysicsManager = new Physics::PhysicsManager(config);
    SceneManager = new Runtime::SceneManager();
}

void RTECore::RunUpdateLoop()
{
    while (true)
    {
        scene.UpdateComponents();
    }
}

RTECore::RTECore()
{
    RTEConfig config;

    // todo: (danh) Sun 21/04 - 11:46: Add raytrace check
    bool raytracingAvailable = true;
    config.UseRaytracing = raytracingAvailable;

    if (ConfigureGame != nullptr)
    {
        ConfigureGame(config);
    }

    // If the user requests raytracing while it is not available, crash
    if (config.UseRaytracing == true && raytracingAvailable == false)
    {
        throw RTEException("Tried to use Raytracing when not available");
    }

    InitEngine(config);

    // Call the client initialize function
    if (OnGameStart != nullptr)
    {
        OnGameStart(*SceneManager);
    }

    RunUpdateLoop();
}

int main(int argc, char const *argv[])
{
    RTECore core;

    return 0;
}
