#include "SwapChain.hpp"

#include <algorithm>
#include <iostream>


#include "Renderpass.hpp"

namespace RTE::Rendering
{

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

VkImageView SwapChain::CreateSwapChainImageView(VkImage image) {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _swapChainImageFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(_instance->GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void SwapChain::CreateSwapChainImages()
{
    vkGetSwapchainImagesKHR(_instance->GetDevice(), _swapChain, &_swapChainImageCount, nullptr);
    _swapChainImages.resize(_swapChainImageCount);
    std::vector<VkImage> vkSwapChainImages;
    vkSwapChainImages.resize(_swapChainImageCount);
    vkGetSwapchainImagesKHR(_instance->GetDevice(), _swapChain, &_swapChainImageCount, vkSwapChainImages.data());

    for(int imageIndex = 0; imageIndex < _swapChainImageCount; imageIndex++)
    {
        ImageInfo &img = _swapChainImages[imageIndex];
        img.imageInfo.image = vkSwapChainImages[imageIndex];
        img.imageInfo.width = _framebufferWidth;
        img.imageInfo.height = _framebufferHeight;
        img.imageView = CreateSwapChainImageView(img.imageInfo.image);
    }
}

void SwapChain::CreateSwapChain()
{
    SupportInformation supportInfo = GetSupportInformation(_instance->GetPhysicalDevice(), _instance->GetSurface());
    
    auto [capabilities, presentModes, extents] = supportInfo;
    
    auto [optFormat, optPresentMode, optExtent] = PickOptimalSwapChainProperties(supportInfo, _framebufferWidth, _framebufferHeight);
    

    //Specify minimum amount of images to function properly
    _swapChainImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && _swapChainImageCount > capabilities.maxImageCount)
    {
        _swapChainImageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _instance->GetSurface();

    createInfo.minImageCount = _swapChainImageCount;
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
    _swapChainImageFormat = optFormat.format;
    _swapChainExtent = optExtent;
    
    CreateSwapChainImages();
}


SwapChain::SwapChain(Instance *instance, int framebufferWidth, int frambufferheight)
{
    _instance = instance;
    _framebufferWidth = framebufferWidth;
    _framebufferHeight = frambufferheight;
    CreateSwapChain();
}

SwapChain::~SwapChain()
{
    for(size_t imageIndex = 0; imageIndex < _swapChainImageCount; imageIndex++)
    {
        vkDestroyFramebuffer(_instance->GetDevice(), _swapChainFramebuffers[imageIndex], nullptr);
        vkDestroyImageView(_instance->GetDevice(), _swapChainImages[imageIndex].imageView, nullptr);
    }


    vkDestroySwapchainKHR(_instance->GetDevice(), _swapChain, nullptr);
}

void SwapChain::CreateFramebuffers(RenderPass *renderPass, ImageManager *imageManager)
{
    _depthImage = imageManager->CreateDepthImage(_framebufferWidth, _framebufferHeight);
    _swapChainFramebuffers.resize(_swapChainImageCount);
    for (size_t i = 0; i < _swapChainImageCount; i++)
    {
        std::array<VkImageView, 2> attachments = {
            _swapChainImages[i].imageView,
            _depthImage.imageView
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->GetHandle();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _swapChainExtent.width;
        framebufferInfo.height = _swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_instance->GetDevice(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
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

std::vector<ImageInfo>& SwapChain::GetSwapChainImages()
{
    return _swapChainImages;
}

uint32_t SwapChain::GetSwapChainImageCount()
{
    return _swapChainImageCount;
}

std::vector<VkFramebuffer>& SwapChain::GetFramebuffers()
{
    return _swapChainFramebuffers;
}

};