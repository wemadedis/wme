#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Instance.hpp"

namespace RTE::Rendering
{

/**
 * @brief Creates commands pools and provides command buffers for rendering, along with one time usage commands buffers.
 * 
 */
class CommandBufferManager
{
private:
    /**
     * @brief Pointer to the instance.
     * 
     */
    Instance *_instance;

    /**
     * @brief Vulkan handle to the command pool
     * 
     */
    VkCommandPool _commandPool;

    /**
     * @brief The command buffers used for draw commands.
     * 
     */
    std::vector<VkCommandBuffer> _commandBuffers;

    /**
     * @brief Creates the command pool used to allocate command buffers.
     * 
     */
    void CreateCommandPool();
    
    

public:
    /**
     * @brief Construct a new Command Buffer Manager object
     * 
     * @param instance Pointer to the instance.
     * @param commandBufferCount The number of command buffers used for draw commands.
     */
    CommandBufferManager(Instance *instance, uint32_t commandBufferCount);

    ~CommandBufferManager();

    /**
     * @brief Begins a one time usage command buffer.
     * 
     * @return Returns a Vulkan handle to a one time use command buffer.
     */
    VkCommandBuffer BeginCommandBufferInstance();

    /**
     * @brief Submits a one time use command buffer to a queue.
     * 
     * @param buffer The one time use command buffer.
     * @param queue THe graphics queue.
     */
    void SubmitCommandBufferInstance(VkCommandBuffer buffer, VkQueue queue);

    /**
     * @brief Get a command buffer for draw command recording.
     * 
     * @param index The index of the command buffer, between 0 and the requested amount of command buffers.
     * @return Returns a Vulkan handle to the command buffer.
     */
    VkCommandBuffer GetCommandBuffer(uint32_t index);

    /**
     * @brief Allocates the draw command buffers.
     * 
     */
    void AllocateCommandBuffers();

    /**
     * @brief Deallocates the draw command buffers.
     * 
     */
    void DeallocateCommandBuffers();

    /**
     * @brief Gets the count of draw command buffers.
     * 
     * @return Returns the number of draw command buffers as an unsigned 32 bit integer.
     */
    uint32_t GetCommandBufferCount();

    /**
     * @brief Gets the command pool.
     * 
     * @return Returns a Vulkan handle to the command pool.
     */
    VkCommandPool GetCommandPool();

};
};