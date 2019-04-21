#include <vector>

#include "rte/GameObject.h"

namespace RTE::Runtime
{

GameObject::GameObject(uint64_t id)
{
    SetId(id);
}

std::vector<Component *> *GameObject::GetComponents()
{
    return _components;
}

uint64_t GameObject::GetId()
{
    return _id;
}

void GameObject::SetId(uint64_t id)
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