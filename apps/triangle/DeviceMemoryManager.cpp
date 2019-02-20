#include "DeviceMemoryManager.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <iostream>
#include <memory>
#include <map>

using namespace std;
namespace DeviceMemoryManager
{

VmaAllocator *alloc = nullptr;
map<VkBuffer, VmaAllocation> buffers;

void InitializeAllocator(VkPhysicalDevice physicalDevice, VkDevice device)
{
    //FIX THIS MALLOC PLS <--------------------------------------------------------------------------------------------------------
    alloc = (VmaAllocator *)malloc(sizeof(VmaAllocator));
    VmaAllocatorCreateInfo info = {};
    info.physicalDevice = physicalDevice;
    info.device = device;
    vmaCreateAllocator(&info, alloc);
}

BufferInformation CreateBuffer(DataType type, MemUsage usage, uint32_t size)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;

    switch (type)
    {
    case DataType::VERTEX:
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case DataType::INDEX:
        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
    case DataType::UNIFORM:
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    }

    VmaAllocationCreateInfo allocInfo = {};
    switch (usage)
    {
    case MemUsage::HOST:
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        break;
    case MemUsage::DEVICE:
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        break;
    }

    VkBuffer buffer;
    VmaAllocation allocation;

    VkResult result = vmaCreateBuffer(*alloc, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("(DeviceMemoryManager) Failed to create buffer!");
    }

    buffers.insert(pair<VkBuffer, VmaAllocation>(buffer, allocation));
    return { type, usage, size, buffer };
}


void DestroyBuffer(BufferInformation bufferInfo)
{
    VmaAllocation allocation = buffers[bufferInfo.buffer];
    vmaDestroyBuffer(*alloc, bufferInfo.buffer, allocation);
    buffers.erase(bufferInfo.buffer);
}

char* GetMemoryState()
{
    return "FECK";
}

} // namespace DeviceMemoryManager