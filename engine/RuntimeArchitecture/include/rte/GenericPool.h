#pragma once

#include <unordered_map>
#include <vector>

template <typename T, int N>
class GenericPool : public ComponentPool
{
  private:
    int _poolSize = 0;

  public:
    T *Pool = nullptr;
    bool *InUse = nullptr;

    T *Get(int index)
    {
        return &Pool[index];
    }
}; 
