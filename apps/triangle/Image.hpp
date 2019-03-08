#pragma once
#include <vulkan/vulkan.h>

#include "DeviceMemoryManager.h"

struct Image
{
    DeviceMemoryManager::ImageInformation imageInfo;
    VkImageView imageView;
};
