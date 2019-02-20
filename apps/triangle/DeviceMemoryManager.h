#pragma once
#include <vulkan/vulkan.h>

namespace DeviceMemoryManager
{
	//Vertex buffer: floats all the way: pos, normal, uvs, etc.
	//Indices: uints all the way
	//Textures: yeah, what? It is kinda uniform, but has specific format.
	//Uniforms: that can be anything, so we need to look at which uniforms we will use in the shaders.

	enum class DATATYPE { VERTEX, INDEX, UNIFORM};
	
	void InitializeAllocator(VkPhysicalDevice physicalDevice, VkDevice device);

	void CreateBuffer(DATATYPE type, uint32_t size);

};
