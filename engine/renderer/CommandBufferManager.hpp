#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Instance.hpp"

namespace RTE::Renderer
{

class CommandBufferManager
{
private:
    Instance *_instance;

    VkCommandPool _commandPool;

    uint32_t _commandBufferCount;
    
    
    std::vector<VkCommandBuffer> _commandBuffers;

    void CreateCommandPool();
    
    

public:
    CommandBufferManager(Instance *instance, uint32_t commandBufferCount);
    ~CommandBufferManager();
    VkCommandBuffer BeginCommandBufferInstance();
    void SubmitCommandBufferInstance(VkCommandBuffer buffer, VkQueue queue);
    VkCommandBuffer GetCommandBuffer(uint32_t index);
    void AllocateCommandBuffers();
    void DeallocateCommandBuffers();
    uint32_t GetCommandBufferCount();
    VkCommandPool GetCommandPool();

};
};