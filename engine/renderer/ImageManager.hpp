#pragma once
#include <vulkan/vulkan.h>

#include "RenderLogicStructs.h"

namespace RTE::Renderer
{

class ImageManager
{
    Instance* _instance;
    CommandBufferManager* _cmdbManager;
public:
    ImageInfo CreateImage();
    VkImageView CreateImageView(DeviceMemoryManager::ImageInformation &imageInfo, VkFormat format, VkImageAspectFlags aspectFlags);
    void TransitionImageLayout(DeviceMemoryManager::ImageInformation &imageInfo, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    TextureInfo CreateTexture(uint32_t width, uint32_t height, unsigned char *pixels, uint32_t byteSize);
    VkSampler CreateSampler();
    ImageInfo CreateDepthImage(uint32_t width, uint32_t height);

    ImageManager(Instance *instance, CommandBufferManager *cmdbManager);
};

};