#pragma once

#include "rte/ECSSystem.h"

class MultiplyByTwoSystem : public ECSSystem
{
private:
	u64 _floatMask = 0;
public:
    MultiplyByTwoSystem(ECSCore* ecs, uint64_t floatMask);
	void Trigger(u64 entity) override;
};