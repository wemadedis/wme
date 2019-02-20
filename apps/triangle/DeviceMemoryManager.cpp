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
VkDevice device;

void InitializeAllocator(VkPhysicalDevice physicalDevice, VkDevice device)
{
    //FIX THIS MALLOC PLS <--------------------------------------------------------------------------------------------------------
    alloc = (VmaAllocator *)malloc(sizeof(VmaAllocator));
    VmaAllocatorCreateInfo info = {};
    info.physicalDevice = physicalDevice;
    info.device = device;
    vmaCreateAllocator(&info, alloc);
    DeviceMemoryManager::device = device;
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

void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkQueue submitQueue) {
		//May need to create a seperate command buffer pool for these short lived buffers
        cout << "lel" << endl;
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(submitQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(submitQueue);
		/*
		A fence would allow you to schedule multiple transfers simultaneously 
		and wait for all of them complete, instead of executing one at a time. 
		That may give the driver more opportunities to optimize.
		*/
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
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