#pragma once

#include "rte/Component.h"

class ComponentPool
{
  public:
    std::vector<Component> Components;
};

template <typename T>
class ComponentPoolInstance : public ComponentPool
{
};