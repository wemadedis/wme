#pragma once

#include <vector>

#include "Component.h"

class GameObject
{
  private:
    std::vector<ComponentId> Components;
};