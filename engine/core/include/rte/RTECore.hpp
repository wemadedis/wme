#pragma once

#include <cstdint>
#include <sstream>
#include <vector>

#include "rte/PhysicsMain.hpp"
#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"
#include "rte/RuntimeAchitectureMain.hpp"

extern void(ConfigureGame)(RTE::RTEConfig &config);
extern void(OnGameStart)(RTE::Runtime::SceneManager &sceneManager);

namespace RTE
{
class RTECore
{

private:
    std::vector<RTEModule *> *Modules = nullptr;
    void InitEngine(RTEConfig &config);
    void RunUpdateLoop();

public:
    static RTEConfig Config;
    RTECore();
};
} // namespace RTE
