#pragma once

#include "rte/Utility.hpp"

#include <cstdint>
#include <stdexcept>

namespace RTE
{
/**
 * @brief 
 *   
 *  Based on http://www.gameprogrammingpatterns.com/object-pool.html
 * 
 * @tparam T 
 */
template <typename T>
class GenericPool
{

private:
    union TWrapper {
        TWrapper *Next = nullptr;
        T Object;
    };

    uint32_t _maxObjects = 0;
    TWrapper *_objects = nullptr;

    TWrapper *_first = nullptr;

public:
    GenericPool(uint32_t count)
    {
        if (count == 0)
        {
            throw RTEException("GenericPool: Attempting to allocate size 0 pool");
        }

        _objects = new TWrapper[count];
        _first = _objects;
        _maxObjects = count;

        // Each object points to the next in the line.
        for (uint32_t i = 0;
             i < count;
             i++)
        {
            _objects[i].Next = _objects + i + 1;
        }

        // The last one terminates the list.
        _objects[count].Next = nullptr;
    }

    ~GenericPool()
    {
        delete _objects;
    }

    T *CreateObject()
    {
        // Remove it from the available list.
        TWrapper *wrapper = _first;
        _first = wrapper->Next;

        if (_first == nullptr)
        {
            throw RTEException("GenericPool: ObjectPool is full!");
        }

        *wrapper = {0};
        return &wrapper->Object;
    }

    void FreeObject(T *obj)
    {
        TWrapper *objWrapper = (TWrapper *)obj;
        objWrapper->Next = _first;
        _first = objWrapper;
    }
};
} // namespace RTE
