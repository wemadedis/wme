#pragma once

#include <set>
#include <unordered_map>
#include <vector>

#include "rte/ComponentPool.h"
#include "rte/Definitions.hpp"
#include "rte/GameObject.h"

namespace RTE::Runtime
{

class Scene
{
private:
    std::vector<ComponentPool *> _componentPools;

public:
    void UpdateComponents(float deltaTime);
    GameObject *CreateGameObject();

    template <typename TComp, int MaxComponents>
    uint64_t DefineComponent()
    {
        ComponentPool *compPool = new ComponentPoolInstance<TComp, MaxComponents>();
        _componentPools.push_back(compPool);
        return _componentPools.size() - 1;
    }

    template <typename TComp>
    TComp *AddComponent(uint64_t compId, GameObject *go)
    {
        Component *comp = _componentPools[compId]->AddComponent(go->GetId());
        TComp *tp = dynamic_cast<TComp *>(comp);
        go->GetComponents()->push_back(tp);
        return tp;
    }
};
} // namespace RTE::Runtime