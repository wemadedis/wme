#include <vector>

#include "rte/Scene.h"
#include "rte/Component.h"
#include "rte/ComponentPool.h"
#include "rte/GameObject.h"

#include "Defs.h"

void Scene::UpdateComponents()
{
    for (i32 componentPoolIndex = 0; componentPoolIndex < _componentPools.size(); componentPoolIndex++)
    {

        _componentPools[componentPoolIndex]->UpdateAll();
    }
}

GameObject* Scene::CreateGameObject()
{
   return new GameObject(0); 
}