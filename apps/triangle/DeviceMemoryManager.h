#pragma once
#include <vulkan/vulkan.h>

namespace DeviceMemoryManager
{
	//Vertex buffer: floats all the way: pos, normal, uvs, etc.
	//Indices: uints all the way
	//Textures: yeah, what? It is kinda uniform, but has specific format.
	//Uniforms: that can be anything, so we need to look at which uniforms we will use in the shaders.

	enum class DataType { VERTEX, INDEX, UNIFORM};
	enum class MemUsage { HOST, DEVICE };

	struct BufferInformation {
		DataType dataType;
		MemUsage memoryUsage;
		uint32_t size;
		VkBuffer buffer;
	};
	

	/*
	Initialize the allocator. Required before calling any other DeviceMemoryManager function.
	*/
	void InitializeAllocator(VkPhysicalDevice physicalDevice, VkDevice device);
	
	/*
	Create a buffer, allocate the necessary memory, and bind the buffer to it.
	*/
	BufferInformation CreateBuffer(DataType type, MemUsage usage, uint32_t size);

	/*
	Copy data from on buffer to another. Used to transfer data from a host visible buffer to a device local buffer.
	*/
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkQueue submitQueue);
	
	/*
	SHOULD IT RETURN A BOOL TO KNOW IF WE DID GOOD, OR CRASH IF WE WANT TO DELETE NON-EXISTENT ITEMS?
	*/
	void DestroyBuffer(BufferInformation bufferInfo);

	/*
	Returns a pointer to a char array describing the current state of memory in JSON format.
	*/
	char* GetMemoryState();
};
