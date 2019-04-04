#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

#include "Utilities.h"

namespace RTE::Rendering
{


class Instance 
{
private:
    VkInstance _instance;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device;
    VkSurfaceKHR _surface;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;
    VkDebugUtilsMessengerEXT _callback;

    const std::vector<const char *> validationLayers = 
    { "VK_LAYER_LUNARG_standard_validation" };

    std::vector<const char *> deviceExtensions = 
    { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    void CreateInstance(std::vector<const char*> &extensions);
    bool DeviceMeetsRequirements(VkPhysicalDevice device);
    void ChoosePhysicalDevice();
    void CreateLogicalDevice();
    void SetupDebugCallBack();
    
    
    
public:
    Instance(std::vector<const char*> &extensions, std::function<void(VkSurfaceKHR &surface, VkInstance instance)> surfaceBindingFunction); 
    ~Instance();
    VkInstance GetInstance();
    VkDevice GetDevice();
    VkQueue GetGraphicsQueue();
    VkQueue GetPresentQueue();
    VkPhysicalDevice GetPhysicalDevice();
    VkSurfaceKHR GetSurface();
    
    VkFormat GetOptimalFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat GetOptimalDepthFormat();
};

};