#pragma once

#include <cstdint>
#include <vector>

#include "rte/PhysicsMain.hpp"
#include "rte/RTEConfig.hpp"
#include "rte/SceneManager.hpp"

extern void(ConfigureGame)(RTEConfig &config);
extern void(OnGameStart)(RTE::Runtime::SceneManager &sceneManager);

class RTECore
{
  private:
    void InitEngine(RTEConfig &config);
    void RunUpdateLoop();

  public:
    RTECore();
};
