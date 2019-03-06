#include "CommandBufferManager.hpp"
#include "Utilities.h"
#include "Instance.hpp"
void CommandBufferManager::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = Utilities::FindQueueFamilies(_instance->GetPhysicalDevice(), _instance->GetSurface());

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
    poolInfo.flags = 0;

    if (vkCreateCommandPool(_instance->GetDevice(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool!");
    }
}


void CommandBufferManager::AllocateCommandBuffers()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = _commandBufferCount;

    if (vkAllocateCommandBuffers(_instance->GetDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}


void CommandBufferManager::DeallocateCommandBuffers()
{
    vkFreeCommandBuffers(_instance->GetDevice(), _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
}

VkCommandBuffer CommandBufferManager::BeginCommandBufferInstance()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_instance->GetDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}


void CommandBufferManager::SubmitCommandBufferInstance(VkCommandBuffer buffer, VkQueue queue)
{
    vkEndCommandBuffer(buffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue); //<---------------------------------------------------- PERFORMANCE IS BAD I THINK

    vkFreeCommandBuffers(_instance->GetDevice(), _commandPool, 1, &buffer);
}


CommandBufferManager::CommandBufferManager(Instance *instance, uint32_t commandBufferCount)
{
    _instance = instance;
    _commandBufferCount = commandBufferCount;
    _commandBuffers.resize(_commandBufferCount);
    CreateCommandPool();
    AllocateCommandBuffers();
}

CommandBufferManager::~CommandBufferManager()
{  
    DeallocateCommandBuffers();
    vkDestroyCommandPool(_instance->GetDevice(), _commandPool, nullptr);
}

VkCommandBuffer CommandBufferManager::GetCommandBuffer(uint32_t index)
{
    if(index < _commandBufferCount)
    {
        return _commandBuffers[index];
    }
    else 
    {
        throw std::runtime_error("CommandBufferManager: Index out of range!");
    }
    
}

uint32_t CommandBufferManager::GetCommandBufferCount()
{
    return _commandBufferCount;
}

VkCommandPool CommandBufferManager::GetCommandPool()
{
    return _commandPool;
}