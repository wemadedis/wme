#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "DeviceMemoryManager.h"
#include "rte/RendererHandles.h"
#include "rte/RenderStructs.h"

namespace RTE::Rendering
{

struct ShaderInfo
{
	ShaderType Type;
	VkShaderModule Module;
};

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
	float Ambient = 0.2f;
    float Diffuse = 0.3f;
    float Specular = 1.0f;
    float Shininess = 10.0f;
    float Reflectivity = 0.0f;
    float Transparency = 0.0f;
	TextureHandle Texture;
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
	glm::vec4 Position;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
    glm::vec4 ClearColor;
	glm::vec4 LightCounts;
	PointLight PointLights[MAX_LIGHTS] = {};
    DirectionalLight DirectionalLights[MAX_LIGHTS] = {};
};

struct MeshInfo
{
	uint64_t VertexCount;
    uint64_t IndexCount;
	BufferInformation vertexBuffer  = {};
	BufferInformation indexBuffer = {};
};

struct MeshInstance
{
	MeshHandle mesh;
	TextureHandle texture;
	BufferInformation uniformBuffer = {};
};

struct VkGeometryInstance
{
    float transform[12];
    uint32_t instanceId : 24;
    uint32_t mask : 8;
    uint32_t instanceOffset : 24;
    uint32_t flags : 8;
    uint64_t accelerationStructureHandle;
};

};