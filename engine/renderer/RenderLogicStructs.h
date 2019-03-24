#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
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
	bool HasTexture;
};

struct DirectionalLight
{
	glm::vec4 Color;
	glm::vec3 Direction;
};

struct PointLight
{
	glm::vec4 Color;
    glm::vec3 Position;
	float Radius;
};

struct GlobalUniformData
{
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
    glm::vec4 AmbientColor;
	PointLight PointLights[10] = {};
    DirectionalLight DirectionalLights[10] = {};
    
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