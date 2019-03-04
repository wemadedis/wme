#pragma once

#include <cstdint>
#include "rte/ECSCore.h"

class ECSCore;
class ECSSystem
{
public:
	ECSCore* _ecsCore = nullptr;
	int _mask = 0;

	ECSSystem(ECSCore* ecsCore);
	virtual void Trigger(int entity) = 0;
	uint32_t GetMask();
	void AddComponent(uint32_t componentMask);
};
