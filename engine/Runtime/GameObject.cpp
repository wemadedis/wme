#include <vector>

#include "rte/Definitions.hpp"
#include "rte/GameObject.h"

namespace RTE::Runtime
{

GameObject::GameObject()
{
    _components = new std::vector<Component *>();
}

std::vector<Component *> *GameObject::GetComponents()
{
    return _components;
}

GameObjectId GameObject::GetId()
{
    return _id;
}

void GameObject::SetId(GameObjectId id)
{
    _id = id;
    _components = new std::vector<Component *>();
}

bool GameObject::GetEnabled()
{
    return _enabled;
}

void GameObject::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

} // namespace RTE::Runtime