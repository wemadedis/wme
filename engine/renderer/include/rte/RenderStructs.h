// TODO: (danh 22/02 09:57): Place this in appropriate Renderer-library when it exists
#pragma once
#include <glm\glm.hpp>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>


namespace RTE::Renderer
{
	/// <summary>Vertex struct for use in RTE rendering.</summary>
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec3 normal;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
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

	enum class LightType {DIRECTIONAL, POINT, AMBIENT};

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


	struct Shader
	{

	};

	struct Material
	{
		Texture* _Texture;
	};
	
	struct Mesh 
	{
		std::vector<Vertex> vertices = {};
		std::vector<uint16_t> indices = {};
	};

	struct RenderPassInfo
	{

	};

	struct RendererSettings
	{

	};
};