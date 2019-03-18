#include <vector>

#include "rte/GameObject.h"
#include "Defs.h"


GameObject::GameObject(u64 id)
{
    SetId(id);
}

std::vector<Component*> *GameObject::GetComponents()
{
    return _components;
}

u64 GameObject::GetId()
{
    return _id;
}

void GameObject::SetId(u64 id)
{
    _id = id;
    _components = new std::vector<Component*>();
}

bool GameObject::GetEnabled()
{
    return _enabled;
}

void GameObject::SetEnabled(bool enabled)
{
    _enabled = enabled;
}
