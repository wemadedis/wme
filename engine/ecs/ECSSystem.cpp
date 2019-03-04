#include "rte/ECSSystem.h"

ECSSystem::ECSSystem(ECSCore* ecsCore)
:_ecsCore(ecsCore)
{}

uint32_t ECSSystem::GetMask()
{
    return _mask;
}

void ECSSystem::AddComponent(uint32_t componentMask)
{
    _mask = _mask | componentMask;
}
