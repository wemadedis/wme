#pragma once

#include <cstdint>
#include <sstream>
#include <vector>

#include "rte/PhysicsMain.hpp"
#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"
#include "rte/RuntimeAchitectureMain.hpp"
#include "rte/WindowManager.h"

extern void(ConfigureGame)(RTE::RTEConfig &config);
extern void(OnGameStart)(RTE::Runtime::SceneManager &sceneManager);

namespace RTE
{
typedef std::function<void(int width, int height)> FrameResizeCallback;
typedef std::function<void(FrameResizeCallback)> SetFrameResizeCallback;

class RTECore
{

private:
    std::vector<RTEModule *> *Modules = nullptr;
    Platform::WindowManager *_windowManager = nullptr;
    void InitEngine();
    void RunUpdateLoop();

public:
    static RTEConfig Config;
    RTECore();
};
} // namespace RTE
