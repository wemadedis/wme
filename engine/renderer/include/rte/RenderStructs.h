// TODO: (danh 22/02 09:57): Place this in appropriate Renderer-library when it exists
#pragma once
#include <glm\glm.hpp>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>

namespace RTE::Renderer
{
/// <summary>Vertex struct for use in RTE rendering.</summary>
struct Vertex
{
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec3 normal;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, texCoord);
        return attributeDescriptions;
    }
};

struct Camera
{
    glm::vec3 Position;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

enum class LightType
{
    DIRECTIONAL,
    POINT,
    AMBIENT
};

struct Light
{
	LightType LightType;
	glm::vec4 Color;
	union
	{
		struct
		{
			glm::vec3 Direction;
		};
		struct
		{
			glm::vec3 Position;
			float Radius;
			float Intensity;
		};
	};
};

struct Texture
{
    unsigned char *Pixels;
    uint16_t Width, Height;
};

// Soon™
struct Shader
{
};

struct Material
{
    float Reflectivity;
    float Diffuse;
    float Specular;
    float Shininess;
    glm::vec4 Color;
    Texture *Texture;
};

struct Mesh
{
    // TODO: (danh 22/03 13:33): Adhere to style guide
    std::vector<Vertex> Vertices = {};
    std::vector<uint16_t> Indices = {};
    // EXTEND: (danh 22/03 13:32): Improve support for nested meshes
    Material Material;
};

struct Transform
{
    glm::vec3 Pos;
    glm::vec3 Scale;
    glm::vec3 Rot;

    Transform() : Transform(glm::vec3(0),glm::vec3(0),glm::vec3(0)) 
    {}

    Transform(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot)
    {
        Pos = pos;
        Scale = scale;
        Rot = rot;
    }

    Transform &operator+(const Transform &other)
    {
        Transform t;
        t.Pos = this->Pos + other.Pos;
        t.Scale = this->Scale * other.Scale;
        t.Rot = this->Rot + other.Rot;
        return t;
    }
};


struct RenderPassInfo
{
};

struct RendererSettings
{
};
}; // namespace RTE::Renderer
