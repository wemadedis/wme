#include "SwapChain.hpp"

#include <algorithm>
#include <iostream>


#include "Renderpass.hpp"

namespace RTE::Rendering
{

SwapChain::SwapChainInformation SwapChain::GetSwapChainInformation(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainInformation swapChainInfo;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainInfo._surfaceCapabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        swapChainInfo._sufraceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainInfo._sufraceFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        swapChainInfo._presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, swapChainInfo._presentModes.data());
    }
    return swapChainInfo;
}


//Where are my tuples?
struct SwapChainProperties
{
    VkSurfaceFormatKHR _surfaceFormat;
    VkPresentModeKHR _presentMode;
    VkExtent2D _extent;
};

SwapChainProperties PickOptimalSwapChainProperties(SwapChain::SwapChainInformation &info, int framebufferWidth, int framebufferHeight)
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
        optimalExtent = {
            static_cast<uint32_t>(framebufferWidth),
            static_cast<uint32_t>(framebufferHeight)};

        optimalExtent.width = std::max(info._surfaceCapabilities.minImageExtent.width, std::min(info._surfaceCapabilities.maxImageExtent.width, optimalExtent.width));
        optimalExtent.height = std::max(info._surfaceCapabilities.minImageExtent.height, std::min(info._surfaceCapabilities.maxImageExtent.height, optimalExtent.height));
    }

    return {optimalFormat, optimalPresentMode, optimalExtent};
}

void SwapChain::CreateSwapChainImageView(Image &image) {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image.Memory.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _swapChainImageFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkResult code = vkCreateImageView(_instance->GetDevice(), &viewInfo, nullptr, &image.View);
    Utilities::CheckVkResult(code, "Failed to create a swap chain image view!");
}

void SwapChain::CreateSwapChainImages()
{
    vkGetSwapchainImagesKHR(_instance->GetDevice(), _swapChain, &_swapChainImageCount, nullptr);
    _swapChainImages.resize(_swapChainImageCount);
    std::vector<VkImage> vkSwapChainImages;
    vkSwapChainImages.resize(_swapChainImageCount);
    vkGetSwapchainImagesKHR(_instance->GetDevice(), _swapChain, &_swapChainImageCount, vkSwapChainImages.data());

    for(uint32_t imageIndex = 0; imageIndex < _swapChainImageCount; imageIndex++)
    {
        Image &img = _swapChainImages[imageIndex];
        img.Memory.image = vkSwapChainImages[imageIndex];
        img.Memory.width = _framebufferWidth;
        img.Memory.height = _framebufferHeight;
        CreateSwapChainImageView(img);
    }
}

void SwapChain::CreateSwapChain()
{
    SwapChainInformation swapChainInfo = GetSwapChainInformation(_instance->GetPhysicalDevice(), _instance->GetSurface());
    
    auto [capabilities, presentModes, extents] = swapChainInfo;
    
    auto [optFormat, optPresentMode, optExtent] = PickOptimalSwapChainProperties(swapChainInfo, _framebufferWidth, _framebufferHeight);
    

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
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

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
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = optPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(_instance->GetDevice(), &createInfo, nullptr, &_swapChain);
    Utilities::CheckVkResult(result, "failed to create swap chain!");
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
        vkDestroyImageView(_instance->GetDevice(), _swapChainImages[imageIndex].View, nullptr);
        _imageManager->DestroyImage(_depthImages[imageIndex]);
    }
    vkDestroySwapchainKHR(_instance->GetDevice(), _swapChain, nullptr);
}

void SwapChain::CreateDepthImages(ImageManager *imageManager)
{
    _depthImages.resize(_swapChainImageCount);
    for(uint32_t imageIndex = 0; imageIndex < _swapChainImageCount; imageIndex++)
    {
        _depthImages[imageIndex] = imageManager->CreateDepthImage(_framebufferWidth, _framebufferHeight);
    }
}

void SwapChain::CreateFramebuffers(RenderPass *renderPass, ImageManager *imageManager)
{
    _imageManager = imageManager;
    /*
     * If the window has been minimized or some exceptional situation arises where both
     * the width or heigh of the frame buffer are 0, set them to 1 to avoid a crash.
     */
    if(_framebufferWidth <= 0 || _framebufferHeight <= 0)
    {
        _framebufferWidth = 1;
        _framebufferHeight = 1;
    }
    CreateDepthImages(imageManager);
    
    _swapChainFramebuffers.resize(_swapChainImageCount);
    for (size_t fbIndex = 0; fbIndex < _swapChainImageCount; fbIndex++)
    {
        std::array<VkImageView, 2> attachments = {
            _swapChainImages[fbIndex].View,
            _depthImages[fbIndex].View
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->GetHandle();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _swapChainExtent.width;
        framebufferInfo.height = _swapChainExtent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(_instance->GetDevice(), &framebufferInfo, nullptr, &_swapChainFramebuffers[fbIndex]);
        Utilities::CheckVkResult(result, "Could not create the frame buffer!");
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

std::vector<Image> SwapChain::GetSwapChainImages()
{
    return _swapChainImages;
}

uint32_t SwapChain::GetSwapChainImageCount()
{
    return _swapChainImageCount;
}

std::vector<VkFramebuffer> SwapChain::GetFramebuffers()
{
    return _swapChainFramebuffers;
}

};