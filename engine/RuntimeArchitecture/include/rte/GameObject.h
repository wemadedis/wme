#pragma once

#include <vector>
#include <cstdint>

#include "rte/Component.h"


/**
 * @brief Represents an object in the game
 * 
 */
class GameObject
{
  private:
    /**
     * @brief The ID of this #GameObject
     */
    uint64_t _id = 0;

    /**
     * @brief std::vector of components attached to this GameObject 
     */
    std::vector<Component*> *_components = nullptr;

    /**
     * @brief The enabled state of this GameObject
     * 
     */
    bool _enabled = true;

  public:
    /**
     * @brief Construct a new #GameObject with the given ID 
     * 
     * @param The ID of this #GameObject, also serves as the index into  
     */
    GameObject(uint64_t id);

    /**
     * @brief Get the #Component's attachted to this game object
     * 
     * @return std::vector<#Component*>* A pointer to the #std::vector containing the attached #Component.
     */
    std::vector<Component*>* GetComponents();

    /**
     * @brief Destroys this #GameObject and associated #Components 
     */
    void Destroy();

    /**
     * @brief Get the ID of this #GameObject
     * 
     * @return u64 The ID of this #GameObject
     */
    uint64_t GetId();

    /**
     * @brief Set the ID of this #GameObject
     * 
     * @param u64 The new ID of this #GameObject
     */
    void SetId(uint64_t id);

    /**
     * @brief Get the enabled state of this #GameObject
     * 
     * @return true If the #GameObject is enabled
     * @return false If the #GameObject is not enabled
     */
    bool GetEnabled();

    /**
     * @brief Set the Enabled state of this #GameObject 
     * 
     * @param enabled The new enabled state of the #GameObject
     */
    void SetEnabled(bool enabled);
};
