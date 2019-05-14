#pragma once

#include <map>
#include <sstream>
#include <vector>

#include "rte/Component.h"
#include "rte/Definitions.hpp"
#include "rte/GameObject.h"
#include "rte/Utility.hpp"

namespace RTE::Runtime
{

class ComponentPool
{
private:
public:
    virtual void UpdateAll(float deltaTime) = 0;
    virtual Component *AddComponent(GameObjectId goId) = 0;
};

template <typename TComp, int MaxComponents>
class ComponentPoolInstance : public ComponentPool
{
public:
    static_assert(std::is_base_of<Component, TComp>::value, "TComp must inherit from Component");
    TComp *Components = nullptr;
    uint64_t ComponentCount = 0;

    ComponentPoolInstance()
    {
        Components = new TComp[MaxComponents];
    }

    void UpdateAll(float deltaTime) override
    {
        for (int32_t componentIndex = 0; componentIndex < ComponentCount; componentIndex++)
        {
            if (Components[componentIndex].GetEnabled())
            {
                Components[componentIndex].Update(deltaTime);
            }
        }
    }

    Component *AddComponent(GameObjectId goId) override
    {
        if (MaxComponents <= ComponentCount)
        {
            std::stringstream ss;
            ss << "Attempting to allocate number " << ComponentCount << " component! Max is " << MaxComponents << std::endl;
            Error(ss.str());
            throw RTEException("Attempting to allocate more components than are available.");
        }
        TComp *c = new (Components + ComponentCount) TComp();
        return &Components[ComponentCount++];
    }
};
} // namespace RTE::Runtime