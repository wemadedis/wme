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
    static GameObjectId goId = 0;
    GameObject *go = new GameObject();
    go->SetId(goId++);
    return go;
}
} // namespace Runtime
} // namespace RTE
