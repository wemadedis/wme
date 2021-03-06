#include "Utilities.h"

#include <set>
#include <string>
#include <vector>

#include "DeviceMemoryManager.h"
#include "RenderLogicStructs.h"
#include "rte/RTEException.h"
#include "rte/RTEProjectProperties.hpp"

namespace RTE::Rendering::Utilities
{

std::vector<char> ReadFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

std::vector<char> ReadEngineAsset(const char *filePath)
{
    std::ostringstream ss;
    ss << ENGINE_ASSET_DIR << filePath;
    return Utilities::ReadFile(ss.str().c_str());
}

VkShaderModule CreateShaderModule(const std::vector<char> &code, VkDevice device)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

ShaderInfo GetStandardVertexShader(VkDevice device)
{

    return {ShaderType::VERTEX, CreateShaderModule(ReadEngineAsset("shaders/vert.spv"), device)};
}

ShaderInfo GetStandardFragmentShader(VkDevice device)
{
    return {ShaderType::FRAGMENT, CreateShaderModule(ReadEngineAsset("shaders/frag.spv"), device)};
}

ShaderInfo GetStandardLineVertexShader(VkDevice device)
{

    return {ShaderType::VERTEX, CreateShaderModule(ReadEngineAsset("shaders/lineVert.spv"), device)};
}

ShaderInfo GetStandardLineFragmentShader(VkDevice device)
{
    return {ShaderType::FRAGMENT, CreateShaderModule(ReadEngineAsset("shaders/lineFrag.spv"), device)};
}

ShaderInfo GetStandardRayGenShader(VkDevice device)
{
    return {ShaderType::RAYGEN, CreateShaderModule(ReadEngineAsset("shaders/rgen.spv"), device)};
}

ShaderInfo GetStandardRayHitShader(VkDevice device)
{
    return {ShaderType::RCHIT, CreateShaderModule(ReadEngineAsset("shaders/rchit.spv"), device)};
}

ShaderInfo GetStandardRayMissShader(VkDevice device)
{
    return {ShaderType::RMISS, CreateShaderModule(ReadEngineAsset("shaders/rmiss.spv"), device)};
}

ShaderInfo GetShadowRayHitShader(VkDevice device)
{
    return {ShaderType::RCHIT, CreateShaderModule(ReadEngineAsset("shaders/srchit.spv"), device)};
}

ShaderInfo GetShadowdRayMissShader(VkDevice device)
{
    return {ShaderType::RMISS, CreateShaderModule(ReadEngineAsset("shaders/srmiss.spv"), device)};
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport)
        {
            indices.PresentFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }

        i++;
    }
    return indices;
}

bool DeviceSupportsExtensions(VkPhysicalDevice device, std::vector<const char *> extensions)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

    for (const auto &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
    }
}

void CheckVkResult(VkResult code, const char *errorMessage)
{
    if (code != VK_SUCCESS)
    {
        errorMessage = (std::string(errorMessage) + std::string(" Code: ") + std::to_string(code)).c_str();
        throw RTEException(errorMessage);
    }
}

}; // namespace RTE::Rendering::Utilities