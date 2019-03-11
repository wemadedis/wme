#pragma once

#include <unordered_map>
#include <vector>

template <typename T, int N>
class GenericPool
{
  private:
    int _poolSize = 0;

  public:
    T *Pool;
    bool *InUse;

    T *Get(int index)
    {
        return &Pool[index];
    }
}; 
