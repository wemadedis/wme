#pragma once

#include <vector>

#include "Defs.h"
#include "rte/ComponentPool.h"
#include "rte/GameObject.h"

class Scene
{
  private:
    /**
     * @brief 
     *  #std::vector of the different ComponentPools 
     */
    std::vector<ComponentPool*> ComponentPools;
  public:
    void UpdateComponents();
    GameObject *CreateGameObject();

    template <typename TComp>
    u64 DefineComponent()
    {
        ComponentPool *compPool = new ComponentPoolInstance<TComp>();
        ComponentPools.push_back(compPool);
        return ComponentPools.size() - 1;

    }

    void AddComponentTo(u64 compId, u64 goID);

#if 0
    /**
     * @brief
     * 
     * @details  Performance concern with dynamic casting
     * @tparam TComp Type of component to get
     * @param goId Id of #GameObject to get component for
     * @return TComp* A pointer to the Component of type #TComp 
     */
    template <typename TComp>
    TComp *Get(u64 goId)
    {
        for(u32 componentPoolIndex  = 0; componentPoolIndex < ComponentPools.size(); componentPoolIndex++)
        {
            ComponentPool curComponent = ComponentPools[componentPoolIndex]; 
            TComp *compPool = dynamic_cast<ComponentPoolInstance<TComp*> (curComponent); 
            if(compPool != nullptr)
            {
                return compPool->get(goId)
            }
        }
    }
#endif

#if 0
    u64 FindFreeEntityIndex()
    {
        for(u64 GOIndex = 0; GOIndex < EntityComponents.size(); GOIndex++)
        {
            if(EntityComponents[GOIndex] != nullptr)
            {
                return GOIndex;
            }
        }
        // TODO: (danh 13/03 10:54): Settle on a way to handle this
        // We did not find a free index, crash here
        throw std::out_of_range("There was no free index for Entity, consider enlarging the Entity pool size");
    }

    Entity *CreateEntity()
    {
        u64 freeIndex = FindFreeEntityIndex();

        // TODO: (danh 13/03 11:14): Object pool this allocation
        EntityComponents[freeIndex] = new std::vector<u64>();

        // TODO: (danh 13/03 10:51): Object pool this allocation
        Entity* go = new Entity(freeIndex);
        return go;
    }
#endif
};