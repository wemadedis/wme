#pragma once

#include <cstdint>

#include "rte/Definitions.hpp"

namespace RTE::Runtime
{
class Component
{
private:
    /**
     * @brief 
     *      Whether or not this component is enabled. Disabled components will not have #Update() called on it.
     */
    bool _enabled = true;

    GameObjectId _gameObjectId = 0;

protected:
    GUIDrawFunction  GUIDraw;

public:
    /**
     * @brief 
     *      Default constructor for Components
     * 
     * @details
     *      A default constructor is required to initialize object pools for the different components
     *      
     *      To initialize a custom component-subclass with non-zero arguments, use an initialization function.
     */
    Component() = default;

    GameObjectId GetGameObjectId();
    void SetGameObjectId(GameObjectId goId);

    /**
     * @brief 
     *      Get the enabled state of this #Component
     * 
     * @return true 
     *      If the #Component is enabled
     * @return false 
     *      If the #Component is not enabled
     */
    bool GetEnabled();

    /**
     * @brief 
     *      Set the enabled state of this #Component 
     * 
     * @param enabled
     *      The new enabled state of the #Component
     */
    void SetEnabled(bool enabled);

    /**
     * @brief
     *      Set the GUI draw function for this Component
     * 
     * @param
     *      The GUI draw function 
     */
    void SetGUIDraw(GUIDrawFunction func);

    /**
     * @brief 
     *      Virtual update function, called every frame unless #GetEnabled returns false.
     */
    virtual void Update(float deltaTime) = 0;
};
} // namespace RTE::Runtime