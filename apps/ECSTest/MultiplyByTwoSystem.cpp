#include "MultiplyByTwoSystem.h"
#include "FloatComponent.h"
#include <vector>
#include <iostream>

MultiplyByTwoSystem::MultiplyByTwoSystem(ECSCore *ecs, uint64_t floatMask)
: ECSSystem(ecs), _floatMask(floatMask)
{
    AddComponent(floatMask);
}

void MultiplyByTwoSystem::Trigger(u64 entity)
{
    auto comp =  _ecsCore->GetComponent<FloatComponent>(entity, _floatMask);
    std::cout << "MultiplyByTwoSystem running! " << comp->value << std::endl;
}
