#include "rte/ECSSystem.h"
#include "RTE.h"
ECSSystem::ECSSystem(ECSCore* ecsCore)
:_ecsCore(ecsCore)
{}

uint32_t ECSSystem::GetMask()
{
    return _mask;
}

void ECSSystem::AddComponent(u64 componentMask)
{
    _mask = _mask | componentMask;
}
