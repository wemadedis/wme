#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include <optional>
#include <fstream>
#include <sstream>




namespace RTE::Rendering 
{
//Must be forward declared (including renderlogicstructs will introduce a circular dependency)
struct ShaderInfo;

namespace Utilities
{


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

std::vector<char> ReadFile(const std::string &filePath);
std::vector<char> ReadEngineAsset(const char* filePath);
VkShaderModule CreateShaderModule(const std::vector<char> &code, VkDevice device);
ShaderInfo GetStandardVertexShader(VkDevice device);
ShaderInfo GetStandardFragmentShader(VkDevice device);
ShaderInfo GetStandardRayGenShader(VkDevice device);
ShaderInfo GetStandardRayHitShader(VkDevice device);
ShaderInfo GetStandardRayMissShader(VkDevice device);
ShaderInfo GetShadowRayHitShader(VkDevice device);
ShaderInfo GetShadowdRayMissShader(VkDevice device);


bool HasStencilComponent(VkFormat format);

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
bool DeviceSupportsExtensions(VkPhysicalDevice device, std::vector<const char*> extensions);
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator);
void CheckVkResult(VkResult code, const char* errorMessage);
};
};