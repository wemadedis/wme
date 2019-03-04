#pragma once

#include "rte/ECSCore.h"
#include "RTE.h"

class ECSCore;
class ECSSystem
{
public:
	ECSCore* _ecsCore = nullptr;
	u64 _mask = 0;

	ECSSystem(ECSCore* ecsCore);
	virtual void Trigger(u64 entity) = 0;
	uint32_t GetMask();
	void AddComponent(u64 componentMask);
};
