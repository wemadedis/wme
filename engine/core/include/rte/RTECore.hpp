#pragma once

#include <cstdint>
#include <vector>

#include "rte/PhysicsManager.hpp"
#include "rte/RTEConfig.hpp"
#include "rte/Scene.h"

// Entry point for
extern void OnGameStart(RTEConfig &config, SceneManager &SceneManager);

struct RequiredManagers
{
    RTE::Physics::PhysicsManager *PhysicsManager;
};
