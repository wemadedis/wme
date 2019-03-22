#pragma once
#include "DeviceMemoryManager.h"

namespace RTE::Renderer
{

struct ImageInfo
{
    ImageInformation imageInfo;
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
	int HasTexture;
};

struct DirectionalLight
{
	glm::vec4 Color;
	glm::vec3 Direction;
};

struct GlobalUniformData
{
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	DirectionalLight Light[10];
};

struct MeshInfo
{
    uint64_t IndexCount;
	BufferInformation vertexBuffer  = {};
	BufferInformation indexBuffer = {};
	BufferInformation uniformBuffer = {};
	TextureInfo *texture = nullptr;
};

};