#pragma once

#include <cstdint>

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
        TWrapper *Next;
        T Object;
    };

    uint32_t _maxObjects = 0;
    TWrapper *_objects;

    TWrapper *_first = nullptr;
    TWrapper *_last = nullptr;

public:
    GenericPool(uint32_t count)
    {
        _objects = new TWrapper[count];
        _first = _objects;
        _last = &_objects[count - 1];
        _maxObjects = count;

        // Each particle points to the next.
        for (uint32_t i = 0;
             i < count - 1;
             i++)
        {
            //_objects[i].Next = &_objects[i + 1];
        }

        // The last one terminates the list.
        //_objects[count - 1].Next = nullptr;
    }

    ~GenericPool()
    {
        delete _objects;
    }

    T *CreateObject()
    {
        // Make sure the pool isn't full.
        assert(_firstAvailable != nullptr);

        // Remove it from the available list.
        TWrapper *wrapper = _first;
        first_ = wrapper->Next;
        return wrapper->Object;
    }

    void FreeObject(T *obj)
    {
        TWrapper *objWrapper = {obj};
        objWrapper->Next = _firstAvailable;
    }
};
} // namespace RTE
