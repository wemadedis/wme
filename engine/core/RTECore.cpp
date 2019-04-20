#pragma once

#include "rte/RTECore.hpp"

#include "rte/RTE.hpp"

#include <cstdint>
#include <vector>

#include "rte/Scene.h"

using namespace RTE;

// TODO: (danh 21/04 00:25): Redo this
static Scene scene;
Scene *CreateScene()
{
    return &scene;
}

static RequiredManagers Managers;

void InitEngine(RTEConfig &config)
{
    Managers.PhysicsManager = new Physics::PhysicsManager(config);
}

int main(int argc, char const *argv[])
{
    RTEConfig config;

    // Call the client initialize function
    OnGameStart(config);

    InitEngine(config);

    while (true)
    {
        scene.UpdateComponents();
    }

    return 0;
}
