#pragma once

#include <vector>
#include <unordered_map>

#include "Defs.h"
#include "rte/ComponentPool.h"
#include "rte/GameObjectPool.h"
#include "rte/GameObject.h"

class Scene
{
  private:
    std::vector<ComponentPool *> _componentPools;
    GameObjectPool _gameObjectPool;

  public:
    void UpdateComponents();
    GameObject *CreateGameObject();

    template <typename TComp>
    u64 DefineComponent()
    {
        ComponentPool *compPool = new ComponentPoolInstance<TComp>();
        _componentPools.push_back(compPool);
        return _componentPools.size() - 1;
    }

    template <typename TComp>
    TComp *AddComponent(u64 compId, GameObject *go)
    {
        Component *comp = _componentPools[compId]->AddComponent(go->GetId());
        comp->SetEnabled(true);
        TComp *tp = static_cast<TComp*>(comp);
        return tp;
    }
};
