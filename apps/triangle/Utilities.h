#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include <optional>
#include "DeviceMemoryManager.h"

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

struct Texture {
	/*
	Vulkan image view.
	*/
	VkImageView imageView;
	/*
	Vulkan sampler specifying how to sample the texture.
	*/
	VkSampler sampler;
	/*
	Information about the texture image.
	*/
	DeviceMemoryManager::ImageInformation imageInformation;
};


struct Mesh {
    std::vector<Vertex> vertices = {};
    std::vector<uint16_t> indices = {};
	DeviceMemoryManager::BufferInformation vertexBuffer  = {};
	DeviceMemoryManager::BufferInformation indexBuffer = {};
	DeviceMemoryManager::BufferInformation uniformBuffer = {};
	Texture texture = {};
	glm::vec3 pos = {0.0f, 0.0f, 0.0f};
	glm::vec3 rot = {0.0f, 0.0f, 0.0f};
	glm::vec3 scale={1.0f, 1.0f, 1.0f};
};


//MODIFY THIS!
struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;

	bool isComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

//AND THIS
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


namespace Utilities
{
	Mesh* MakeCylinder(float radius, float height, int faces);
	Texture CreateTexture(uint32_t width, uint32_t height, unsigned char *pixels, uint32_t byteSize, VkCommandBuffer commandBuffer, VkDevice device);
	void TransitionImageLayout(DeviceMemoryManager::ImageInformation imageInfo, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer &commandBuffer);
	bool HasStencilComponent(VkFormat format);
	VkImageView CreateImageView(DeviceMemoryManager::ImageInformation &imageInfo, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device);
	VkSampler CreateSampler(VkDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool DeviceSupportsExtensions(VkPhysicalDevice device, std::vector<const char*> extensions);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator);
}

