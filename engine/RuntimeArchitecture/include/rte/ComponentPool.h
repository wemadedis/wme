#pragma once

#include <map>
#include <vector>

#include "rte/Component.h"
#include "Defs.h"

class ComponentPool
{
    std::map<u64, u64> _goIdToComponentIndex;
    u64 GetFreeComponentIndex();

  public:
    std::vector<Component> Components = std::vector<Component>(1000);
    Component *AddComponent(u64 goId);
    void UpdateAll();
    std::map<u64, u64> *GetGOIdToComponentIndex();
};

template <typename T>
class ComponentPoolInstance : public ComponentPool
{
};