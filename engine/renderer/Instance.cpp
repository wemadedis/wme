#include "Instance.hpp"
#include "SwapChain.hpp"
#include "Utilities.h"
#include <set>
#include <iostream>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace RTE::Rendering
{


void Instance::CreateInstance(std::vector<const char*> &extensions)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    #ifndef NDEBUG
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    #else
    createInfo.ppenabledLayerNames = 0;
    #endif

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

bool Instance::DeviceMeetsRequirements(VkPhysicalDevice device)
{
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    Utilities::QueueFamilyIndices indices = Utilities::FindQueueFamilies(device, _surface);
    bool extensionsSupported = Utilities::DeviceSupportsExtensions(device, _neededDeviceExtensions);


    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChain::SupportInformation swapChainSupport = SwapChain::GetSupportInformation(device, _surface);
        swapChainAdequate = !swapChainSupport._sufraceFormats.empty() && !swapChainSupport._presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

void Instance::ChoosePhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
    bool supportsRT = false;
    for (const auto &device : devices)
    {
        if (DeviceMeetsRequirements(device))
        {
            _physicalDevice = device;
            if(_rayTracingCapable)
            {
                supportsRT = Utilities::DeviceSupportsExtensions(device, {VK_NV_RAY_TRACING_EXTENSION_NAME});
                if(supportsRT) 
                {
                    break;
                }
            } else 
            {
                break;
            }
            
        }
    }
    _rayTracingCapable = supportsRT;
    if (_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void Instance::CreateLogicalDevice()
{
    Utilities::QueueFamilyIndices indices = Utilities::FindQueueFamilies(_physicalDevice, _surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
    std::set<uint32_t> uniqueQueueFamilies = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(_neededDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = _neededDeviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }
    VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }
    vkGetDeviceQueue(_device, indices.GraphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, indices.PresentFamily.value(), 0, &_presentQueue);
}





static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}


void Instance::SetupDebugCallBack()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    if (Utilities::CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_callback) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

Instance::Instance(std::vector<const char*> &extensions, std::function<void(VkSurfaceKHR &surface, VkInstance instance)> surfaceBindingFunction, bool isRayTracing)
{
    if(isRayTracing)
    {
        _rayTracingCapable = isRayTracing;
    }
    CreateInstance(extensions);
    surfaceBindingFunction(_surface, _instance);
    ChoosePhysicalDevice();
    CreateLogicalDevice();
    
    #ifndef NDEBUG
    SetupDebugCallBack();
    #endif
}

Instance::~Instance()
{
    #ifndef NDEBUG
    Utilities::DestroyDebugUtilsMessengerEXT(_instance, _callback, nullptr);
    #endif

    vkDestroyDevice(_device, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);
}

VkInstance Instance::GetInstance()
{
    return _instance;
}


VkDevice Instance::GetDevice()
{
    return _device;
}

VkQueue Instance::GetGraphicsQueue()
{
    return _graphicsQueue;
}

VkQueue Instance::GetPresentQueue()
{
    return _presentQueue;
}

VkPhysicalDevice Instance::GetPhysicalDevice()
{
    return _physicalDevice;
}

VkSurfaceKHR Instance::GetSurface()
{
    return _surface;
}


VkFormat Instance::GetOptimalFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {

        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}


VkFormat Instance::GetOptimalDepthFormat()
{
return GetOptimalFormat(
    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
    VK_IMAGE_TILING_OPTIMAL,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool Instance::IsRayTracingCapable()
{
    return _rayTracingCapable;
}


};