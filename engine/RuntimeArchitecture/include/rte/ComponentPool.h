#pragma once

#include <map>
#include <vector>

#include "rte/Component.h"
namespace RTE::Runtime
{

class ComponentPool
{
private:
public:
    virtual void UpdateAll(float deltaTime) = 0;
    virtual Component *AddComponent(uint64_t goId) = 0;
};

template <typename TComp>
class ComponentPoolInstance : public ComponentPool
{
public:
    static_assert(std::is_base_of<Component, TComp>::value, "TComp must inherit from Component");
    std::vector<TComp*> Components = std::vector<TComp*>();
    std::map<uint64_t, uint64_t> _goIdToComponentIndex;

    void UpdateAll(float deltaTime) override
    {
        for (int32_t componentIndex = 0; componentIndex < Components.size(); componentIndex++)
        {
            if (Components[componentIndex]->GetEnabled())
            {
                Components[componentIndex]->Update(deltaTime);
            }
        }
    }

    uint64_t GetFreeComponentIndex()
    {
        return Components.size();
    }

    Component *AddComponent(uint64_t goId) override
    {
        static_assert(std::is_base_of<Component, TComp>::value, "TComp must inherit from Component");
        // Add gameobject index to map index
        uint64_t componentIndex = GetFreeComponentIndex();
        _goIdToComponentIndex[goId] = componentIndex;
		TComp *c = new TComp();
		c->GameObjectID = goId;
        Components.push_back(c);
        return Components[componentIndex];
    }
};
} // namespace RTE::Runtime