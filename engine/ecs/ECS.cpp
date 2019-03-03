#include <iostream>
#include "rte/ECS.h"


void ECSCore::Update()
{
    for (int entityIndex = 0; entityIndex < EntityCount; entityIndex++)
    {
        for(int i = 0; i < Components.size(); i++)
        {
            if (Mask[entityIndex] == Components[i].GetMask())
            {
                Components[i].Trigger();
            }
        }
    }
}

unsigned int ECSCore::CreateEntity()
{
    for (unsigned int entity = 0; entity < EntityCount; entity++)
    {
        if (Mask[entity] == 0)
        {
            return (entity);
        }
    }
    return EntityCount;
}

void ECSCore::DestroyEntity(unsigned int entity)
{
    Mask[entity] = 0;
}
