#include <vector>

#include "rte/Scene.h"
#include "rte/Component.h"
#include "rte/ComponentPool.h"
#include "rte/GameObject.h"

void Scene::UpdateComponents()
{
    for (int32_t componentPoolIndex = 0; componentPoolIndex < _componentPools.size(); componentPoolIndex++)
    {
        _componentPools[componentPoolIndex]->UpdateAll();
    }
}

GameObject* Scene::CreateGameObject()
{
   return new GameObject(0); 
}