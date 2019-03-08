#pragma once

#include <unordered_map>
#include <vector>

class ComponentPool
{
};


template <typename T, int N>
class GenericPool : public ComponentPool
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


void Initialize();

void main()
{
    
}

class MemoryManager
{
  public:
    MemoryManager();
    std::vector<ComponentPool*> ComponentPool;

    template <typename TObject, int N = 10>
    GenericPool<TObject, N> *Allocate()
    {
        auto pool = new GenericPool<TObject, N>();
        ComponentPool.push_back(pool)
        return pool;
    }
};
