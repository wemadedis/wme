#include <vector>

#include "rte/Component.h"
#include "rte/ComponentPool.h"
#include "rte/GameObject.h"
#include "rte/Scene.h"

namespace RTE
{
namespace Runtime
{
void Scene::UpdateComponents()
{
    for (int32_t componentPoolIndex = 0; componentPoolIndex < _componentPools.size(); componentPoolIndex++)
    {
        _componentPools[componentPoolIndex]->UpdateAll();
    }
}

GameObject *Scene::CreateGameObject()
{
    // todo: (danh) Mon 22/04 - 01:26: Fix this
    return new GameObject(0);
}

} // namespace Runtime
} // namespace RTE
