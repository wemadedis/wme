#include "rte/Component.h"

#include "rte/RenderingManager.h"

namespace RTE::Runtime
{
bool Component::GetEnabled()
{
    return _enabled;
}

void Component::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

void Component::SetGUIDraw(GUIDrawFunction func)
{
    GUIDraw = func;
    auto rm = Rendering::RenderingManager::GetInstance();
    rm->RegisterGUIDrawFunction(this, func);
}

GameObjectId Component::GetGameObjectId()
{
    return _gameObjectId;
}

void Component::SetGameObjectId(GameObjectId goId)
{
    _gameObjectId = goId;
}

} // namespace RTE::Runtime