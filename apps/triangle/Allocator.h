#pragma once
#include <vulkan/vulkan.h>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

static VmaAllocator alloc;

namespace Allocator
{
	//Vertex buffer: floats all the way: pos, normal, uvs, etc.
	//Indices: uints all the way
	//Textures: yeah, what? It is kinda uniform, but has specific format.
	//Uniforms: that can be anything, so we need to look at which uniforms we will use in the shaders.

	enum class DATATYPE { VERTEX, INDEX, UNIFORM};
	

	void InitializeAllocator(VkPhysicalDevice physicalDevice, VkDevice device) {
		VmaAllocatorCreateInfo info = {};
		info.physicalDevice = physicalDevice;
		info.device = device;
		vmaCreateAllocator(&info, &alloc);
	}

	void CreateBuffer(DATATYPE type, uint32_t size) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;

		switch (type) {
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

		vmaCreateBuffer(alloc, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
		
	}

	void UploadDataToBuffer(DATATYPE type, VkBuffer buffer, void* data) {
		
	}
};
