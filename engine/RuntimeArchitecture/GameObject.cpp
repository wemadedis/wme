#include "rte/GameObject.h"
#include "Defs.h"

GameObject::GameObject(u64 id)
{
    SetId(id);
}

u64 GameObject::GetId()
{
    return Id;
}

void GameObject::SetId(u64 id)
{
    Id = id;
}