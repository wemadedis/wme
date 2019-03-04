#include "MultiplyByTwoSystem.h"
#include "FloatComponent.h"
#include <vector>
#include <iostream>

MultiplyByTwoSystem::MultiplyByTwoSystem(ECSCore *ecs, uint64_t floatMask)
: ECSSystem(ecs)
{
    AddComponent(floatMask);
}

void MultiplyByTwoSystem::Trigger(int entity)
{
    //std::vector<ECSComponent>* ecscomp = _ecsCore->Components[GetMask()];
    //FloatComponent comp = reinterpret_cast<FloatComponent>(ecscomp->at(entity));
    std::cout << "MultiplyByTwoSystem running!"  << std::endl;
}