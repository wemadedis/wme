#pragma once
#include <vulkan/vulkan.h>

#include "Instance.hpp"
#include "DeviceMemoryManager.h"
#include "CommandBufferManager.hpp"
#include "Texture.hpp"


class ImageManager
{
    Instance* _instance;
    CommandBufferManager* _cmdbManager;
public:
    VkImageView CreateImageView(DeviceMemoryManager::ImageInformation &imageInfo, VkFormat format, VkImageAspectFlags aspectFlags);
    void TransitionImageLayout(DeviceMemoryManager::ImageInformation &imageInfo, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    Texture CreateTexture(uint32_t width, uint32_t height, unsigned char *pixels, uint32_t byteSize);
    VkSampler CreateSampler();


    ImageManager(Instance *instance, CommandBufferManager *cmdbManager);
};