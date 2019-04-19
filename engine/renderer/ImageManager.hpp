#pragma once
#include <vulkan/vulkan.h>

#include "RenderLogicStructs.h"

namespace RTE::Rendering
{

class ImageManager
{
    Instance* _instance;
    CommandBufferManager* _cmdbManager;
    DeviceMemoryManager* _deviceMemoryManager;
public:
    ImageInfo CreateImage();
    VkImageView CreateImageView(ImageInformation &imageInfo, VkFormat format, VkImageAspectFlags aspectFlags);
    void TransitionImageLayout(ImageInformation &imageInfo, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    TextureInfo CreateTexture(uint32_t width, uint32_t height, unsigned char *pixels, uint32_t byteSize);
    VkSampler CreateSampler();
    ImageInfo CreateDepthImage(uint32_t width, uint32_t height);

    //RT
    void ImageBarrier(VkCommandBuffer commandBuffer, VkImage image, VkImageSubresourceRange& subresourceRange,
    VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout);

    ImageManager(Instance *instance, CommandBufferManager *cmdbManager, DeviceMemoryManager *memoryManager);
};

};