#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "DeviceMemoryManager.h"
#include "RendererHandles.h"
namespace RTE::Rendering
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
	glm::vec4 Color = glm::vec4(0.5, 0.5, 0.5, 1.0f);
	bool HasTexture;
};

const uint32_t MAX_LIGHTS = 10;

struct DirectionalLight
{
	glm::vec4 Color;
	glm::vec4 Direction;
};

struct PointLight
{
	glm::vec4 Color;
    glm::vec4 PositionRadius;
};

struct GlobalUniformData
{
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
    glm::vec4 AmbientColor;
	glm::vec4 LightCounts;
	PointLight PointLights[MAX_LIGHTS] = {};
    DirectionalLight DirectionalLights[MAX_LIGHTS] = {};
};

struct MeshInfo
{
    uint64_t IndexCount;
	BufferInformation vertexBuffer  = {};
	BufferInformation indexBuffer = {};
	BufferInformation uniformBuffer = {};
	TextureInfo *texture = nullptr;
};

struct MeshInstance
{
	MeshHandle mesh;
	TextureHandle texture;
};

};