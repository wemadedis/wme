#pragma once

#include "rte/RTECore.hpp"
#include "rte/RTE.hpp"
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

static RequiredManagers Managers;

void RTECore::InitEngine(RTEConfig &config)
{
    Managers.PhysicsManager = new Physics::PhysicsManager(config);
    Managers.SceneManager = new Runtime::SceneManager();
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
    if (ConfigureGame != nullptr)
    {
        ConfigureGame(config);
    }
    InitEngine(config);

    // Call the client initialize function
    if (OnGameStart != nullptr)
    {
        OnGameStart(*Managers.SceneManager);
    }

    RunUpdateLoop();
}

int main(int argc, char const *argv[])
{
    RTECore core;

    return 0;
}
