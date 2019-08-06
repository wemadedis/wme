#include "ImageManager.hpp"

namespace RTE::Rendering
{

VkImageView ImageManager::CreateImageView(ImageMemory &imageInfo, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = imageInfo.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
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

void ImageManager::TransitionImageLayout(ImageMemory &imageInfo, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //Old layout can be set to VK_IMAGE_LAYOUT_UNDEFINED if the content of the image is unimportant
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    //Next two are used if transferring queue family ownership. Otherwise must be set to VK_QUEUE_FAMILY_IGNORED
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //image and subresourceRange specify the affected image and its specific parts.
    barrier.image = imageInfo.image;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        //Check if format has stencil component
        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    //No mipmap levels
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    /**
     * For synchronization purposes: specify which types of operations involving the resource happens
     * before the barrier, and which operations must wait on the barrier.
    */
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }
    VkCommandBuffer commandBuffer = _cmdbManager->BeginCommandBufferInstance();
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0, //Either 0 or VK_DEPENDENCY_BY_REGION_BIT allowing to read data if that part is written to
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    _cmdbManager->SubmitCommandBufferInstance(commandBuffer, _instance->GetGraphicsQueue());
}

TextureInfo ImageManager::CreateTexture(uint32_t width, uint32_t height, unsigned char *pixels, uint32_t size){
    Buffer stagingBuffer;
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemProps::HOST, size, stagingBuffer);
    _deviceMemoryManager->CopyDataToBuffer(stagingBuffer, pixels);
    
    ImageMemory texture = _deviceMemoryManager->CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT); 
    
    TransitionImageLayout(texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    _deviceMemoryManager->CopyBufferToImage(stagingBuffer, texture);

    TransitionImageLayout(texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    _deviceMemoryManager->DestroyBuffer(stagingBuffer);
    VkImageView imageView = CreateImageView(texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
    return {_stdSampler, {texture, imageView, _stdSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}};
}

VkSampler ImageManager::CreateSampler() 
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    VkSampler sampler;
    if (vkCreateSampler(_instance->GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a sampler!");
    }
    return sampler;
}

Image ImageManager::CreateDepthImage (uint32_t width, uint32_t height) {
    VkFormat depthFormat = _instance->GetOptimalDepthFormat();
    ImageMemory imgInfo = _deviceMemoryManager->CreateImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    VkImageView imgview = CreateImageView(imgInfo, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    TransitionImageLayout(imgInfo, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    return {imgInfo, imgview, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL};
}

void ImageManager::DestroyImage(Image &image)
{
    vkDestroyImageView(_instance->GetDevice(), image.View, nullptr);
    _deviceMemoryManager->DestroyImage(image.Memory);
}

ImageManager::ImageManager(Instance *instance, CommandBufferManager *cmdbManager, DeviceMemoryManager *memoryManager)
{
    _instance = instance;
    _cmdbManager = cmdbManager;
    _deviceMemoryManager = memoryManager;
    _stdSampler = CreateSampler();
}


};