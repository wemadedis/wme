#include <vector>

#include "rte/Scene.h"
#include "rte/Component.h"
#include "rte/ComponentPool.h"
#include "rte/GameObject.h"

#include "Defs.h"

void Scene::UpdateComponents()
{
    for (i32 componentPoolIndex = 0; componentPoolIndex < ComponentPools.size(); componentPoolIndex++)
    {
        ComponentPool *pool = ComponentPools[componentPoolIndex];

        for (i32 componentIndex = 0; componentIndex < pool->Components.size(); componentIndex++)
        {
            pool->Components[componentIndex].Update();
        }
    }
}

GameObject *Scene::CreateGameObject()
{
    return new GameObject(1);
}

void Scene::AddComponentTo(u64 compId, u64 goID)
{
    //ComponentPools[goID]->Components.push_back(compId);
}
