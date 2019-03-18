#pragma once

#include <map>
#include <vector>

#include "rte/Component.h"
#include "Defs.h"

class ComponentPool
{
  private:
  public:
    virtual void UpdateAll() = 0;
    virtual Component *AddComponent(u64 goId) = 0;

};

template <typename TComp>
class ComponentPoolInstance : public ComponentPool
{
  public:
    static_assert(std::is_base_of<Component, TComp>::value, "TComp must inherit from Component");
    std::vector<TComp> Components = std::vector<TComp>();
    std::map<u64, u64> _goIdToComponentIndex;

    void UpdateAll() override
    {
        for (i32 componentIndex = 0; componentIndex < Components.size(); componentIndex++)
        {
            if (Components[componentIndex].GetEnabled())
            {
                Components[componentIndex].Update();
            }
        }
    }

    u64 GetFreeComponentIndex()
    {
        return Components.size();
    }

    Component *AddComponent(u64 goId) override
    {
        static_assert(std::is_base_of<Component, TComp>::value, "TComp must inherit from Component");
        // Add gameobject index to map index
        u64 componentIndex = GetFreeComponentIndex();
        _goIdToComponentIndex[goId] = componentIndex;
        Components.push_back(TComp());
        Components[componentIndex].SetEnabled(true);
        return &Components[componentIndex];
    }
};