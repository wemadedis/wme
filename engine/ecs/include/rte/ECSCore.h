#pragma once

#include <vector>
#include <unordered_map>

#include "rte/ECSSystem.h"
#include "rte/ECSComponent.h"
#include "Defs.h"

class ECSSystem;
class ECSCore
{
  public:
    typedef u64 EntityType;

	static const uint32_t EntityCount = 10;

	u64 Mask[EntityCount];

	std::unordered_map<u64, std::vector<ECSComponent>*> Components;

	std::vector<ECSSystem *> Systems;

	void AddSystem(ECSSystem *system);

	inline bool FitsSystem(EntityType entityMask, EntityType systemMask);

    template <typename T>
	EntityType AddComponent(T a);

    template <typename T>
    T* GetComponent(EntityType entity, u64 component);

	void Update();

	uint64_t GetFreeEntity();

	void DestroyEntity(EntityType entity);
};

template <typename T>
uint64_t ECSCore::AddComponent(T a)
{
    i32 mask = 1 << Components.size();
    std::vector<ECSComponent>* v = new std::vector<ECSComponent>();

    v->resize(EntityCount);
    Components.insert(std::make_pair(mask, v));

	return static_cast<uint64_t>(mask);
}

template<typename T>
T*  ECSCore::GetComponent(u64 entity, u64 component) {
    std::vector<T>* compList = (std::vector<T>*)Components[component];
    T comp = (T)(compList->at(entity));
    return &comp;
}
