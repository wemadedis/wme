#include <iostream>
#include <cstdint>
#include <rte/ECSCore.h>

#include "rte/ECSCore.h"
#include "Defs.h"

void ECSCore::Update()
{
    for (u32 entityIndex = 0; entityIndex < EntityCount; entityIndex++)
    {
        for(u32 i = 0; i < Systems.size(); i++)
        {
            if (FitsSystem(Mask[entityIndex], Systems[i]->GetMask()))
            {
                Systems[i]->Trigger(entityIndex);
            }
        }
    }
}

void ECSCore::AddSystem(ECSSystem* system)
{
    Systems.push_back(system);
}

inline bool ECSCore::FitsSystem(u64 entityMask, u64 systemMask)
{
    return (entityMask & systemMask) == systemMask;
}

uint64_t ECSCore::GetFreeEntity()
{
    for (u32 entityIndex = 0; entityIndex < EntityCount; entityIndex++)
    {
        if (Mask[entityIndex] == 0)
        {
            return (entityIndex);
        }
    }
    return EntityCount;
}

void ECSCore::DestroyEntity(u64 entityIndex)
{
    Mask[entityIndex] = 0;
}