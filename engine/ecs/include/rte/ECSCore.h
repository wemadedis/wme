#pragma once

#include <vector>
#include <map>

#include "rte/ECSSystem.h"
#include "rte/ECSComponent.h"

class ECSSystem;

class ECSComponentCollection
{
  public:
	ECSComponentCollection(uint32_t size, uint64_t mask);
};

ECSComponentCollection::ECSComponentCollection(uint32_t size, uint64_t mask)
{
	
}


class ECSCore
{
  public:
	static const uint32_t EntityCount = 10;

	uint64_t Mask[EntityCount];
	std::map<uint64_t, ECSComponentCollection> Components;
	std::vector<ECSSystem *> Systems;

	void AddSystem(ECSSystem *system);

	template <typename T>
	uint64_t AddComponent();

	void Update();

	uint64_t GetFreeEntity();

	void DestroyEntity(uint64_t entity);
};

template <typename T>
uint64_t ECSCore::AddComponent()
{
	uint64_t mask = 1 << Components.size();

	//std::vector<ECSComponentCollection> *v = new std::vector<T>(EntityCount);

	//Components.insert(mask, v);

	return mask;
}
