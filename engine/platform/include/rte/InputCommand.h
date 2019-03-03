#pragma once
#include "rte/InputManager.h"

namespace RTE::InputManager
{
class InputCommand
{
protected:
  union InputData *data = nullptr;

public:
  virtual void Execute() = 0;
};
}; // namespace RTE::InputManager
