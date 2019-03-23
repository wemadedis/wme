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
	bool HasTexture;
};

struct DirectionalLight
{
	glm::vec4 Color;
	glm::vec3 Direction;
};

struct PointLight
{
    glm::vec3 Position;
	glm::vec4 Color;
	glm::vec3 Radius;
};

struct GlobalUniformData
{
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
    glm::vec4 AmbientColour;
    DirectionalLight DirLight[10];
    PointLight PointLight[10];
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