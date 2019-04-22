#pragma once
#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>
#include <iostream>
#include <memory>
#include <map>


#include "CommandBufferManager.hpp"

namespace RTE::Rendering
{
enum class MemProps { HOST, DEVICE };

/*
Struct describing the buffer created through the CreateBuffer function.
Do not modify the content.
*/
struct BufferInformation {
	VkBufferUsageFlags bufferUsage;
	MemProps memoryProperties;
	size_t size;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkBufferView BufferView = VK_NULL_HANDLE;
};

/*
Struct describing the image created through the CreateImage function.
Do not modify the content.
*/
struct ImageInformation {
	MemProps memoryProperties;
	uint32_t width;
	uint32_t height;
	/*
	Size of the image in bytes.
	*/
	uint32_t size; 
	VkImage image;
};


class DeviceMemoryManager
{
	VmaAllocator *_allocator = nullptr;
	std::map<VkBuffer, VmaAllocation> _buffers;
	std::map<VkImage, VmaAllocation> _images;
	std::map<VkAccelerationStructureNV, VkDeviceMemory> _accelerationStructures;
	Instance* _instance;
	CommandBufferManager *_cmdbManager;
	VkPhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties;
public:

	/*
	Initialize the allocator. Required before calling any other DeviceMemoryManager function.
	*/
	DeviceMemoryManager(Instance *instance, CommandBufferManager *commandBufferManager);
	
	/*
	Create a buffer, allocate the necessary memory, and bind the buffer to it.
	*/
	void CreateBuffer(VkBufferUsageFlags bufferUsage, MemProps props, size_t size, BufferInformation& bufferInfo);

	
	/*
	Copies data from the host memory to a host visible buffer by mapping the buffer memory.
	*/
	void CopyDataToBuffer(BufferInformation& bufferInfo, void* data);

	template<typename T>
	void ModifyBufferData(BufferInformation& bufferInfo, std::function<void(T*)> Mutator)
	{
		void* mapping = malloc(bufferInfo.size); //<--------------------------------------- TRIED TO FREE IT AFTER UNMAP, GOT EXCEPTION (IS UNMAP FREEING IT IMPLICITLY??)
		VmaAllocation& allocation = _buffers[bufferInfo.buffer];
		vmaMapMemory(*_allocator, allocation, (void**)&mapping);
		T* before = (T*)mapping;
		Mutator(before);
		vmaUnmapMemory(*_allocator, allocation);
	}

	/*
	Copy data from on buffer to another. Used to transfer data from a host visible buffer to a device local buffer.
	*/
	void CopyBuffer(BufferInformation& srcBuffer, BufferInformation& dstBuffer, size_t size, VkCommandPool commandPool, VkQueue submitQueue);
	
	/*
	Destroy the buffer and deallocate the memory used by it.
	*/
	void DestroyBuffer(BufferInformation& bufferInfo);

	/*
	Create an image allocated on the device.
	*/
	ImageInformation CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
	
	/*
	Copy data from a buffer to an image.
	*/
	void CopyBufferToImage(BufferInformation &srcBuffer, ImageInformation &dstImage, uint32_t width, uint32_t height, VkCommandBuffer &commandBuffer);

	void DestroyImage(ImageInformation& imageInfo);

	uint32_t GetMemoryType(VkMemoryRequirements& memoryRequiriments, VkMemoryPropertyFlags memoryProperties);

	void AllocateAccelerationStructureMemory(VkAccelerationStructureNV &AS);

	void CreateScratchBuffer(std::vector<VkAccelerationStructureNV> &bot, VkAccelerationStructureNV &top, BufferInformation &buffer);

	/*
	Returns a pointer to a char array describing the current state of memory in JSON format.
	*/
	char* GetMemoryState();
};
};
