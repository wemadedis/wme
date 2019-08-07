#pragma once
#include <vulkan/vulkan.h>

#include "RenderLogicStructs.h"

namespace RTE::Rendering
{
/**
 * @brief Encapsulates functions related to image and texture creation/modification. 
 */
class ImageManager
{
    /**
     * @brief Pointer to the instance.
     */
    Instance* _instance;

    /**
     * @brief Pointer to the command buffer manager 
     */
    CommandBufferManager* _cmdbManager;
    
    /**
     * @brief Pointer to the device memory manager 
     */
    DeviceMemoryManager* _deviceMemoryManager;

    /**
     * @brief Handle to a sampler used for all textures.
     */
    VkSampler _stdSampler;
public:

    /**
     * @brief Create an image view based on previously allocated image resource.
     * @param imageInfo The allocated image using the Device Memory Manager.
     * @param format The image view format to be used.
     * @param aspectFlags Mask of image aspect flag bits.
     * @return Returns the an image view with the requested properties.
     */
    VkImageView CreateImageView(ImageMemory &imageInfo, VkFormat format, VkImageAspectFlags aspectFlags);

    /**
     * @brief Transitions the layout of an image from its old layout to a new one.
     * @param imageInfo The image allocation information.
     * @param format The image format
     * @param oldLayout The old image layout
     * @param newLayout The new image layout
     */
    void TransitionImageLayout(ImageMemory &imageInfo, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    
    /**
     * @brief Uploads texture image data to the GPU and creates a texture info.
     * @param width The width of the texture image.
     * @param height The height of the texture image.
     * @param pixels The image pixel data.
     * @param size The size of the image, in bytes.
     * @return Returns a TextureInfo struct containing necessary information for rendering.
     */
    Image CreateTexture(uint32_t width, uint32_t height, unsigned char *pixels, uint32_t size);

    /**
     * @brief Creates a sampler for texture sampling.
     * @return Returns a Vulkan handle to a sampler
     */
    VkSampler CreateSampler();

    /**
     * @brief Creates a depth image to be used by the frame buffer.
     * @param width The width of the depth image to be created.
     * @param height The height of the depth image to be created.
     */
    Image CreateDepthImage(uint32_t width, uint32_t height);

    /**
     * @brief Deallocates the memory of the image and deletes the image view.
     * @param image The Image object containing a valid image and an image view.
     */
    void DestroyImage(Image &image);

    /**
     * @brief Default constructor of the Image Manager.
     * @param instance The instance of the renderer.
     * @param cmdbManager The command buffer manager.
     * @param memoryManager The device memory manager.
     */
    ImageManager(Instance *instance, CommandBufferManager *cmdbManager, DeviceMemoryManager *memoryManager);
};

};