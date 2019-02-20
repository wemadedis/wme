#include "DeviceMemoryManager.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <iostream>
#include <memory>
VmaAllocator *alloc = nullptr;


void DeviceMemoryManager::InitializeAllocator(VkPhysicalDevice physicalDevice, VkDevice device)
{
    //FIX THIS MALLOC PLS <--------------------------------------------------------------------------------------------------------
    alloc = (VmaAllocator*)malloc(sizeof(VmaAllocator));
    VmaAllocatorCreateInfo info = {};
    info.physicalDevice = physicalDevice;
    info.device = device;
    std::cout << alloc << std::endl;
    vmaCreateAllocator(&info, alloc);
    std::cout << alloc << std::endl;
}

void DeviceMemoryManager::CreateBuffer(DATATYPE type, uint32_t size)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;

    switch (type)
    {
    case DATATYPE::VERTEX:
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case DATATYPE::INDEX:
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
    case DATATYPE::UNIFORM:
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    }
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkBuffer buffer;
    VmaAllocation allocation;
    
    VkResult result = vmaCreateBuffer(*alloc, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("(DeviceMemoryManager) Failed to create buffer!");
    }
}