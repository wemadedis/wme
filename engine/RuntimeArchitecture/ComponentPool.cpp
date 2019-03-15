#include "rte/ComponentPool.h"

///////////////////////////////
//////      PRIVATE      //////
///////////////////////////////

u64 ComponentPool::GetFreeComponentIndex()
{
    return Components.size() - 1;
}

//////////////////////////////
//////      PUBLIC      //////
//////////////////////////////

Component* ComponentPool::AddComponent(u64 goId)
{
    // Add gameobject index to map index
    u64 componentIndex = GetFreeComponentIndex();
    _goIdToComponentIndex[goId] = componentIndex;

    // TODO: (danh 15/03 14:45): This might not be the right time to enable the component
    Components[componentIndex].SetEnabled(true);
    return &Components[componentIndex];
}

void ComponentPool::UpdateAll()
{    
    for (i32 componentIndex = 0; componentIndex < Components.size(); componentIndex++)
    {
        if(Components[componentIndex].GetEnabled())
        {
            Components[componentIndex].Update();
        }   
    }
}

std::map<u64, u64> *ComponentPool::GetGOIdToComponentIndex()
{
    return &_goIdToComponentIndex;
}