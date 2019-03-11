#pragma once

#include "MemoryManager.h"

class Scene
{
  private:
    
  public:
    template <typename TComp>
    void DefineComponent(int MaxCount)
    {
        // Okay we're trying to define an object pool of type TComp
        // TComp has a type! Surprising, no?
        // So what we're trying to do, is generate a THING that can handle THAT
        auto floatPool = new GenericPool<float, 100>();
        float *f = floatPool->Get(1);
    }
};