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

/**
 * @brief Encapsulates the graphics and present queue family indices.
 */
struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;

	/**
	 * @brief Checks if both family indices are present.
	 * @return True If the both the graphics and present family indices are present.
	 * @return False If one of the indices is not present.
	 */
	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

/**
 * @brief Reads a file's content.
 * @param filePath The absolute file path.
 * @return The file contents as a char vector.
 */
std::vector<char> ReadFile(const std::string &filePath);

/**
 * @brief Reads a file's content.
 * @param filePath The relative file path to the specified asset folder.
 * @return The file contents as a char vector.
 */
std::vector<char> ReadEngineAsset(const char* filePath);

/**
 * @brief Creates a shader module from shader code.
 * @param code The shader code.
 * @param device Vulkan handle to the logical device.
 * @return The shader module.
 */
VkShaderModule CreateShaderModule(const std::vector<char> &code, VkDevice device);

/**
 * @brief Creates a ShaderInfo struct from a standard vertex shader file.
 * @param device Vulkan handle to the logical device.
 * @return ShaderInfo containing the shader module and the shader type.
 */
ShaderInfo GetStandardVertexShader(VkDevice device);

/**
 * @brief Creates a ShaderInfo struct from a standard fragment shader file.
 * @param device Vulkan handle to the logical device.
 * @return ShaderInfo containing the shader module and the shader type.
 */
ShaderInfo GetStandardFragmentShader(VkDevice device);

/**
 * @brief Creates a ShaderInfo struct from a standard ray generator shader file.
 * @param device Vulkan handle to the logical device.
 * @return ShaderInfo containing the shader module and the shader type.
 */
ShaderInfo GetStandardRayGenShader(VkDevice device);

/**
 * @brief Creates a ShaderInfo struct from a standard ray closest hit shader file.
 * @param device Vulkan handle to the logical device.
 * @return ShaderInfo containing the shader module and the shader type.
 */
ShaderInfo GetStandardRayHitShader(VkDevice device);

/**
 * @brief Creates a ShaderInfo struct from a standard ray miss file.
 * @param device Vulkan handle to the logical device.
 * @return ShaderInfo containing the shader module and the shader type.
 */
ShaderInfo GetStandardRayMissShader(VkDevice device);

/**
 * @brief Creates a ShaderInfo struct from a standard shadow ray closest hit file.
 * @param device Vulkan handle to the logical device.
 * @return ShaderInfo containing the shader module and the shader type.
 */
ShaderInfo GetShadowRayHitShader(VkDevice device);

/**
 * @brief Creates a ShaderInfo struct from a standard shadow ray miss file.
 * @param device Vulkan handle to the logical device.
 * @return ShaderInfo containing the shader module and the shader type.
 */
ShaderInfo GetShadowdRayMissShader(VkDevice device);

/**
 * @brief Attempts to get the graphics and present queue family indices.
 * @param device Vulkan handle to the logical device.
 * @param surface The surface used by the renderer.
 * @return QueueFamily struct with each index, if availavle.
 */
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * @brief Checks whether a device supports the reuquested extensions.
 * @param device Vulkan handle to the logical device.
 * @param extensions Vector of the requested extension names.
 * @return true if the device supports the extensions.
 * @return false if the device does not support the extensions.
 */
bool DeviceSupportsExtensions(VkPhysicalDevice device, std::vector<const char*> extensions);

/**
 * @brief Creates a Vulkan debug messenger
 */
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback);


/**
 * @brief Destroys a Vulkan debug messenger.
 */
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator);


/**
 * @brief Checks a Vulkan result. If it is not a VK_SUCCESS, it throws an exception.
 * @param code The result to check.
 * @param errorMessage Message to be provided with the exception.
 */
void CheckVkResult(VkResult code, const char* errorMessage);
};
};