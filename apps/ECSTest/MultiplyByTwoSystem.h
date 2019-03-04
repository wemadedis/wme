#pragma once

#include "rte/ECSSystem.h"

class MultiplyByTwoSystem : public ECSSystem
{
public:
    MultiplyByTwoSystem(ECSCore* ecs, uint64_t floatMask);
	void Trigger(int entity);
};