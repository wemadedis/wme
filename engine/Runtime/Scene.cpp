#include <vector>

#include "rte/Component.h"
#include "rte/ComponentPool.h"
#include "rte/GameObject.h"
#include "rte/Scene.h"

namespace RTE
{
namespace Runtime
{
void Scene::UpdateComponents(float deltaTime)
{
    for (int32_t componentPoolIndex = 0; componentPoolIndex < _componentPools.size(); componentPoolIndex++)
    {
        _componentPools[componentPoolIndex]->UpdateAll(deltaTime);
    }
}

GameObject *Scene::CreateGameObject()
{
    GameObject go;
    go.SetId(_gameObjects.size());
    _gameObjects.push_back(go);
    return &_gameObjects[_gameObjects.size() - 1];
}

} // namespace Runtime
} // namespace RTE
