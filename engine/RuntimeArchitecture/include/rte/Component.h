#pragma once

class Component
{
  private:
    /**
     * @brief 
     *      Whether or not this component is enabled. Disabled components will not have #Update() called on it.
     */
    bool _enabled = false;

  public:
    /**
     * @brief 
     *      Default constructor for Components
     * 
     * @details
     *      A default constructor is required to initialize object pools for the different components
     *      
     *      To initialize a custom component-subclass, use an initialization function.
     */
    Component() = default;

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
     *      The new enabled state of the #Compenent
     */
    void SetEnabled(bool enabled);

    /**
     * @brief 
     *      Virtual update function, called every frame unless #GetEnabled returns false.
     */
    virtual void Update() {}
};