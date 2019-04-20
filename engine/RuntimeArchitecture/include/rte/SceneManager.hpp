#pragma once

#include "rte/Scene.h"

namespace RTE
{
namespace Runtime
{
class SceneManager
{
  public:
    Scene *MakeScene();
};

} // namespace Runtime
} // namespace RTE
