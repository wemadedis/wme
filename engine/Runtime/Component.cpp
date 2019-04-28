#include "rte/Component.h"

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

GameObjectId Component::GetGameObjectId()
{
    return _gameObjectId;
}

void Component::SetGameObjectId(GameObjectId goId)
{
    _gameObjectId = goId;
}

} // namespace RTE::Runtime