#pragma once
#include "DeviceMemoryManager.h"

namespace RTE::Renderer
{

struct ImageInfo
{
    DeviceMemoryManager::ImageInformation imageInfo;
    VkImageView imageView;
};

struct TextureInfo
{
	/*
	Vulkan sampler specifying how to sample the texture.
	*/
	VkSampler sampler;
	/*
	Information about the texture image.
	*/
	ImageInfo image;
};

struct MeshUniformData
{
	glm::mat4 ModelMatrix;
};

struct GlobalUniformData
{
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct MeshInfo
{
    uint64_t IndexCount;
	DeviceMemoryManager::BufferInformation vertexBuffer  = {};
	DeviceMemoryManager::BufferInformation indexBuffer = {};
	DeviceMemoryManager::BufferInformation uniformBuffer = {};
	TextureInfo texture = {};
};

};