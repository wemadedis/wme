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
    // todo: (danh) Mon 22/04 - 01:26: Fix this
    static uint64_t id = 0;
    return new GameObject(id++);
}

} // namespace Runtime
} // namespace RTE
