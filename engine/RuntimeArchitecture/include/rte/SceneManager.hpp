#pragma once

#include "rte/RTEModule.hpp"
#include "rte/Scene.h"

namespace RTE::Runtime
{
class SceneManager : public RTEModule
{
private:
    Scene *_activeScene = nullptr;

public:
    SceneManager();
    void Update(float deltaTime) override;
    Scene *MakeScene();
    Scene *GetActiveScene();
    void SetActiveScene(Scene *scene);
};
} // namespace RTE::Runtime
