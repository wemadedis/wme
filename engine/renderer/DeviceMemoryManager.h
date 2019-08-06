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

/**
 * @brief Struct describing a buffer allocation.
 * 
 */
struct Buffer {
	VkBufferUsageFlags bufferUsage;
	MemProps memoryProperties;
	size_t size;
	VkBuffer buffer = VK_NULL_HANDLE;
};

/**
 * @brief Struct describing an image allocation.
 * 
 */
struct ImageMemory {
	MemProps memoryProperties;
	uint32_t width;
	uint32_t height;
	uint32_t size; 
	VkImage image;
};

/**
 * @brief Manages memory allocation for buffer and image resources, along with data transfer to the resources.
 * 
 */
class DeviceMemoryManager
{
	/**
	 * @brief Pointer to the Vulkan Memory Allocator allocator.
	 * 
	 */
	VmaAllocator *_allocator = nullptr;

	/**
	 * @brief Mapping from Vulkan buffer handles to the VMA allocations.
	 * 
	 */
	std::map<VkBuffer, VmaAllocation> _buffers;

	/**
	 * @brief Mapping from Vulkan image handles to the VMA allocations.
	 * 
	 */
	std::map<VkImage, VmaAllocation> _images;

	/**
	 * @brief Mapping from Vulkan acceleration structure handles to device memory handles.
	 * 
	 */
	std::map<VkAccelerationStructureNV, VkDeviceMemory> _accelerationStructures;

	/**
	 * @brief Pointer to the instance.
	 * 
	 */
	Instance* _instance;

	/**
	 * @brief Pointer to the command buffer manager.
	 * 
	 */
	CommandBufferManager *_cmdbManager;

	/**
	 * @brief Struct describing the physical device memory properties.
	 * 
	 */
	VkPhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties;
public:

	/**
	 * @brief Constructor for the Device Memory Manager.
	 * 
	 * @param instance Pointer to the instance.
	 * @param commandBufferManager Pointer to the command buffer manager.
	 */
	DeviceMemoryManager(Instance *instance, CommandBufferManager *commandBufferManager);
	
	/*
	Create a buffer, allocate the necessary memory, and bind the buffer to it.
	*/

	/**
	 * @brief Creates a buffer resource and allocates memory for it.
	 * 
	 * @param bufferUsage The usage of the buffer.
	 * @param props The memory property describing if the memory should be allocated on the host or the device.
	 * @param size The size of the buffer, in bytes.
	 * @param bufferInfo The buffer information object used to store the allocation information.
	 */
	void CreateBuffer(VkBufferUsageFlags bufferUsage, MemProps props, size_t size, Buffer& bufferInfo);

	/**
	 * @brief Copies data to a buffer
	 * 
	 * @param bufferInfo Buffer allocation information.
	 * @param data Pointer to the data to be copied
	 * @param size The size of the data to be copied. If 0, uses the allocation information size.
	 */
	void CopyDataToBuffer(Buffer& bufferInfo, void* data, uint64_t size = 0);

	/**
	 * @brief Modifies data in a buffer in the host space.
	 * 
	 * @tparam T The type of the data to be modified.
	 * @param bufferInfo The buffer allocation information.
	 * @param Mutator Function modifying the data based on the specified data type.
	 */
	template<typename T>
	void ModifyBufferData(Buffer& bufferInfo, std::function<void(T*)> Mutator)
	{
		void* mapping;// = malloc(bufferInfo.size); //<--------------------------------------- TRIED TO FREE IT AFTER UNMAP, GOT EXCEPTION (IS UNMAP FREEING IT IMPLICITLY??)
		VmaAllocation& allocation = _buffers[bufferInfo.buffer];
		vmaMapMemory(*_allocator, allocation, (void**)&mapping);
		T* before = (T*)mapping;
		Mutator(before);
		vmaUnmapMemory(*_allocator, allocation);
	}

	/**
	 * @brief Copy data from on buffer to another. Used to transfer data from a host visible buffer to a device local buffer.
	 * 
	 * @param srcBuffer Allocation information of the source buffer.
	 * @param dstBuffer Allocation information of the destination buffer
	 * @param size The size of the data to be copied.
	 * @param commandPool Vulkan handle to a command pool.
	 * @param submitQueue Queue to submit the copy command to.
	 */
	void CopyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, size_t size, VkCommandPool commandPool, VkQueue submitQueue);

	/**
	 * @brief Destroys a buffer and deallocates the related memory.
	 * 
	 * @param bufferInfo The buffer allocation information.
	 */
	void DestroyBuffer(Buffer& bufferInfo);

	/*
	Create an image allocated on the device.
	*/

	/**
	 * @brief Creates a Vulkan image resource and allocates memory for it.
	 * 
	 * @param width The with of the image.
	 * @param height The height of the image.
	 * @param format The format of the image.
	 * @param tiling The tiling arrangement of the image data.
	 * @param usage  The usage of the image.
	 * @return ImageMemory The image information object used to store the allocation information.
	 */
	ImageMemory CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

	/**
	 * @brief Copies data from a buffer to an image.
	 * 
	 * @param srcBuffer Source buffer allocation information.
	 * @param dstImage Allocation information on the destination image.
	 * @param commandBuffer Command buffer used for the copy command.
	 */
	void CopyBufferToImage(Buffer &srcBuffer, ImageMemory &dstImage);


	/**
	 * @brief Destroys the image resource and deallocates the related memory.
	 * 
	 * @param imageInfo The image information object.
	 */
	void DestroyImage(ImageMemory& imageInfo);

	/**
	 * @brief Finds the memory type based on a memory requirement and properies.
	 * 
	 * @param memoryRequiriments The memory requirements.
	 * @param memoryProperties The memory properties.
	 * @return Returns the memory type as an unsigned 32 bit integer.
	 */
	uint32_t GetMemoryType(VkMemoryRequirements& memoryRequiriments, VkMemoryPropertyFlags memoryProperties);

	/**
	 * @brief Allocates memory of an acceleration structure.
	 * 
	 * @param AS The acceleration structure.
	 */
	void AllocateAccelerationStructureMemory(VkAccelerationStructureNV &AS);

	/**
	 * @brief Creates a Scratch Buffer and allocates memory for it. Used for acceleration structures.
	 * 
	 * @param bot The bottom level acceleration structures.
	 * @param top The top level acceleration structure.
	 * @param buffer The buffer information object used to store the allocation information.
	 */
	void CreateScratchBuffer(std::vector<VkAccelerationStructureNV> &bot, VkAccelerationStructureNV &top, Buffer &buffer);
};
};
