#pragma once

#include <vector>


class ECSCore
{
  public:
	static const int EntityCount = 10;

	std::vector<class ECSComponent> Components;


	int Mask[EntityCount];

	void Update();
	unsigned int CreateEntity();
	void DestroyEntity(unsigned int entity);
};

class ECSComponent
{
  private:
	ECSCore *_ecsCore;

  public:
	virtual int GetMask() = 0;
	virtual void Trigger() = 0;
};
