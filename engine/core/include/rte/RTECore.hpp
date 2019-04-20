#pragma once

#include <cstdint>
#include <vector>

#include "rte/PhysicsManager.hpp"
#include "rte/RTEConfig.hpp"
#include "rte/SceneManager.hpp"

extern void (*ConfigureGame)(RTEConfig &config) = nullptr;
extern void (*OnGameStart)(RTE::Runtime::SceneManager &sceneManager) = nullptr;

struct RequiredManagers
{
    RTE::Physics::PhysicsManager *PhysicsManager;
    RTE::Runtime::SceneManager *SceneManager;
};