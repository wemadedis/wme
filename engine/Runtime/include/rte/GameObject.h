#pragma once

#include <cstdint>
#include <vector>

#include "rte/Component.h"
#include "rte/Definitions.hpp"

namespace RTE::Runtime
{

/**
 * @brief Represents an object in the game
 * 
 */
class GameObject
{
private:
    /// @brief The ID of this GameObject
    GameObjectId _id = 0;

    /// @brief The enabled state of this GameObject
    bool _enabled = true;

    /// @brief std::vector of #RTE::Runtime::Component's attached to this GameObject
    std::vector<Component *> *_components = nullptr;

public:
    /**
     * @brief Construct a new GameObject with the given ID 
     * 
     * @param id The ID of this GameObject
     */
    GameObject(GameObjectId id);

    /**
     * @brief Get the #RTE::Runtime::Component's attachted to this GameObject
     * 
     * @return std::vector<#RTE::Runtime::Component*>* A pointer to the std::vector containing the attached #RTE::Runtime::Component's.
     */
    std::vector<Component *> *GetComponents();

    /**
     * @brief Get the ID of this GameObject
     * 
     * @return uint64_t The ID of this GameObject
     */
    uint64_t GetId();

    /**
     * @brief Set the ID of this GameObject
     * 
     * @param id The new ID of this GameObject
     */
    void SetId(uint64_t id);

    /**
     * @brief Get the enabled state of this GameObject
     * 
     * @return true If the GameObject is enabled
     * @return false If the GameObject is not enabled
     */
    bool GetEnabled();

    /**
     * @brief Set the Enabled state of this GameObject 
     * 
     * @param enabled The new enabled state of the GameObject
     */
    void SetEnabled(bool enabled);
};

} // namespace RTE::Runtime