#pragma once
#include <vector>
#include "rte/GameObject.h"

class GameObjectPool
{
  public:
    std::vector<GameObject> GameObjects;
};


template <typename T>
class GameObjectPoolInstance : public GameObjectPool
{
};