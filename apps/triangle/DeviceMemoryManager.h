#pragma once
#include <vulkan/vulkan.h>


#include "CommandBufferManager.hpp"

namespace DeviceMemoryManager
{
	//Vertex buffer: floats all the way: pos, normal, uvs, etc.
	//Indices: uints all the way
	//Textures: yeah, what? It is kinda uniform, but has specific format.
	//Uniforms: that can be anything, so we need to look at which uniforms we will use in the shaders.

	enum class MemProps { HOST, DEVICE };


	/*
	Struct describing the buffer created through the CreateBuffer function.
	Do not modify the content.
	*/
	struct BufferInformation {
		VkBufferUsageFlags bufferUsage;
		MemProps memoryProperties;
		size_t size;
		VkBuffer buffer;
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
	

	/*
	Initialize the allocator. Required before calling any other DeviceMemoryManager function.
	*/
	void Initialize(Instance *instance, CommandBufferManager *commandBufferManager);
	
	/*
	Create a buffer, allocate the necessary memory, and bind the buffer to it.
	*/
	void CreateBuffer(VkBufferUsageFlags bufferUsage, MemProps props, size_t size, BufferInformation& bufferInfo);

	
	/*
	Copies data from the host memory to a host visible buffer by mapping the buffer memory.
	*/
	void CopyDataToBuffer(BufferInformation& bufferInfo, void* data);


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
	/*
	Returns a pointer to a char array describing the current state of memory in JSON format.
	*/
	char* GetMemoryState();
};
