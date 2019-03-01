#pragma once
#include <vulkan/vulkan.h>

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
		VkImage image;
	};
	

	/*
	Initialize the allocator. Required before calling any other DeviceMemoryManager function.
	*/
	void Initialize(VkPhysicalDevice physicalDevice, VkDevice device);
	
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
	SHOULD IT RETURN A BOOL TO KNOW IF WE DID GOOD, OR CRASH IF WE WANT TO DELETE NON-EXISTENT ITEMS?
	*/
	void DestroyBuffer(BufferInformation& bufferInfo);

	/*
	Create an image allocated on the device.
	*/
	//void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
	ImageInformation CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

	void DestroyImage(ImageInformation& imageInfo);
	/*
	Returns a pointer to a char array describing the current state of memory in JSON format.
	*/
	char* GetMemoryState();
};
