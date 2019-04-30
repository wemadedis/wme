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

    TWrapper *_firstAvailable = nullptr;

public:
    GenericPool(uint32_t count)
    {
        _objects = new TWrapper[count];
        _firstAvailable = _objects;
        _maxObjects = count;

        // Each particle points to the next.
        for (uint32_t i = 0;
             i < count - 1;
             i++)
        {
            _objects[i].Next = &_objects[i + 1];
        }

        // The last one terminates the list.
        _objects[count - 1].Next = nullptr;
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
        TWrapper *wrapper = _firstAvailable;
        firstAvailable_ = wrapper->Next;
        return _firstAvailable;
    }

    void FreeObject(T *obj)
    {
        TWrapper *objWrapper = (TWrapper *)obj;
        objWrapper->Next = _firstAvailable;
    }
};
} // namespace RTE
