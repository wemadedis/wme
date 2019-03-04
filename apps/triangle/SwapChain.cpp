#include "SwapChain.hpp"
#include <algorithm>

SwapChain::SupportInformation SwapChain::GetSupportInformation(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SupportInformation details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details._surfaceCapabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details._sufraceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details._sufraceFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details._presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details._presentModes.data());
    }
    return details;
}


//Where are my tuples?
struct {VkSurfaceFormatKHR _surfaceFormat;
        VkPresentModeKHR _presentMode;
        VkExtent2D _extent;} PickOptimalSwapChainProperties(SwapChain::SupportInformation &info, int framebufferWidth, int framebufferHeight)
{
    VkSurfaceFormatKHR optimalFormat;
    VkPresentModeKHR optimalPresentMode;
    VkExtent2D optimalExtent;

    //Get optimal format
    if (info._sufraceFormats.size() == 1 && info._sufraceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        optimalFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto &availableFormat : info._sufraceFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            optimalFormat = availableFormat;
            break;
        }
        else
        {
            optimalFormat =  info._sufraceFormats[0]; //Possible to go through the formats and choose the best, but it also works with the first one available.
        }
    }
    

    //Get optimal present mode
    optimalPresentMode = VK_PRESENT_MODE_FIFO_KHR; //This mode is guaranteed?

    for (const auto &availablePresentMode : info._presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        { //if this mode is available choose it
            optimalPresentMode = availablePresentMode;
            break;
        }
        else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            optimalPresentMode = availablePresentMode;
        }
    }

    //Get optimal extent
    if (info._surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			optimalExtent = info._surfaceCapabilities.currentExtent;
		}
		else
		{
			int width, height;
			//glfwGetFramebufferSize(window, &width, &height); <------------------------------------------------------------------------------------------------------------------------------

			optimalExtent = {
				static_cast<uint32_t>(framebufferWidth),
				static_cast<uint32_t>(framebufferHeight)};

			optimalExtent.width = std::max(info._surfaceCapabilities.minImageExtent.width, std::min(info._surfaceCapabilities.maxImageExtent.width, optimalExtent.width));
			optimalExtent.height = std::max(info._surfaceCapabilities.minImageExtent.height, std::min(info._surfaceCapabilities.maxImageExtent.height, optimalExtent.height));
    }

    return {optimalFormat, optimalPresentMode, optimalExtent};
}

void SwapChain::CreateSwapChain()
{
    SupportInformation supportInfo = GetSupportInformation(_instance->GetPhysicalDevice(), _instance->GetSurface());
    
    auto [capabilities, presentModes, extents] = supportInfo;
    
    auto [optFormat, optPresentMode, optExtent] = PickOptimalSwapChainProperties(supportInfo, _framebufferWidth, _framebufferHeight);
    

    //Specify minimum amount of images to function properly
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _instance->GetSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = optFormat.format;
    createInfo.imageColorSpace = optFormat.colorSpace;
    createInfo.imageExtent = optExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto [graphicsFamily, presentFamily] = Utilities::FindQueueFamilies(_instance->GetPhysicalDevice(), _instance->GetSurface());
    uint32_t queueFamilyIndices[] = {graphicsFamily.value(), presentFamily.value()};

    if (graphicsFamily != presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;	 // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = optPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_instance->GetDevice(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(_instance->GetDevice(), _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_instance->GetDevice(), _swapChain, &imageCount, _swapChainImages.data());
    _swapChainImageFormat = optFormat.format;
    _swapChainExtent = optExtent;
}


SwapChain::SwapChain(Instance *instance, int framebufferWidth, int frambufferheight)
{
    _instance = instance;
    _framebufferWidth = framebufferWidth;
    _framebufferHeight = frambufferheight;
    CreateSwapChain();
}

VkSwapchainKHR SwapChain::GetSwapChain()
{
    return _swapChain;
}

VkFormat SwapChain::GetSwapChainImageFormat()
{
    return _swapChainImageFormat;
}

VkExtent2D SwapChain::GetSwapChainExtent()
{
    return _swapChainExtent;
}

std::vector<VkImage>& SwapChain::GetSwapChainImages()
{
    return _swapChainImages;
}