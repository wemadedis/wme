#include <iostream>
#include <cstdint>

#include "rte/ECSCore.h"

void ECSCore::Update()
{
    // TODO: (danh) Sun 03/03 - 18:06: parallellise this
    for (uint32_t entityIndex = 0; entityIndex < EntityCount; entityIndex++)
    {
        for(uint32_t i = 0; i < Systems.size(); i++)
        {
            if (Mask[entityIndex] == Systems[i]->GetMask())
            {
                Systems[i]->Trigger(i);
            }
        }
    }
}

void ECSCore::AddSystem(ECSSystem* system)
{
    Systems.push_back(system);
}

uint32_t ECSCore::GetFreeEntity()
{
    for (uint32_t entityIndex = 0; entityIndex < EntityCount; entityIndex++)
    {
        if (Mask[entityIndex] == 0)
        {
            return (entityIndex);
        }
    }
    return EntityCount;
}

void ECSCore::DestroyEntity(uint32_t entityIndex)
{
    Mask[entityIndex] = 0;
}
