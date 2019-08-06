#pragma once
#include <vector>


#include <vulkan/vulkan.h>

#include "Instance.hpp"
#include "ImageManager.hpp"

namespace RTE::Rendering
{


class RenderPass;

class SwapChain
{

private:
	/**
	 * @brief 
	 */
	Instance* _instance;

	/**
	 * @brief Pointer to the image manager for dapth image creation and deletion.
	 */
	ImageManager *_imageManager;

	/**
	 * @brief Width of the frame buffer.
	 */
	uint32_t _framebufferWidth;

	/**
	 * @brief Height of the frame buffer.
	 */
	uint32_t _framebufferHeight;

	/**
	 * @brief Vulkan handle to the swap chain.
	 */
	VkSwapchainKHR _swapChain;

	/**
	 * @brief The number of swap chain images.
	 */
	uint32_t _swapChainImageCount;

	/**
	 * @brief The swap chain images.
	 */
	std::vector<Image> _swapChainImages;

	/**
	 * @brief Format of the swap chain images.
	 */
	VkFormat _swapChainImageFormat;

	/**
	 * @brief The extent of the swap chain images.
	 */
	VkExtent2D _swapChainExtent;

	/**
	 * @brief Vulkan handles to frambe buffer objects.
	 */
	std::vector<VkFramebuffer> _swapChainFramebuffers;

	/**
	 * @brief The depth images used for the depth buffers.
	 */
	std::vector<Image> _depthImages;

	/**
	 * @brief Creates the depth images used by the frame buffers.
	 * @param imageManager The image manager used for creating the images.
	 */
	void CreateDepthImages(ImageManager *imageManager);
	
	/**
	 * @brief Creates the swap chain images.
	 */
	void CreateSwapChainImages();

	/**
	 * @brief Creates an image view for a swap chain image.
	 * @param image The Vulkan handle to an image.
	 * @return Returns a Vulkan handle to the image view.
	 */
	VkImageView CreateSwapChainImageView(VkImage image);
	
	/**
	 * @brief Encapsulates the Vulkan swap chain creation process.
	 */
	void CreateSwapChain();

public:

	/**
	 * @brief Contains necessary information about the rendering surface. 
	 */
	struct SwapChainInformation 
	{
		/**
		 * @brief Vulkan struct describing the surface capabilities.
		 */
		VkSurfaceCapabilitiesKHR _surfaceCapabilities;

		/**
		 * @brief The supported surface formats.
		 */
		std::vector<VkSurfaceFormatKHR> _sufraceFormats;

		/**
		 * @brief The supported present modes
		 */
		std::vector<VkPresentModeKHR> _presentModes;
	};

	/**
	 * @brief Retrieves the swap chain information on surface capabilities, formats, and present modes.
	 * @param physicalDevice Vulkan handle to the physical device used by the renderer.
	 * @param surface The surface used by the renderer.
	 */
    static SwapChainInformation GetSwapChainInformation(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	/**
	 * @brief The swap chain cosntructor.
	 * @param instance
	 * @param framebufferWidth The requested width of the frame buffer.
	 * @param frameBufferHeight The requested height of the frame buffer.
	 */
	SwapChain(Instance *instance, int framebufferWidth, int frameBufferHeight);
	
	~SwapChain();

	/**
	 * @brief Creates the frame buffers used for rendering.
	 * @param renderPass Pointer to a render pass.
	 * @param imageManager Pointer to an image manager.
	 */
	void CreateFramebuffers(RenderPass *renderPass, ImageManager *imageManager);

	/**
	 * @brief Gets the swap chain handle.
	 * @return Returns a Vulkan handle to the swap chain.
	 */
	VkSwapchainKHR GetSwapChain();

	/**
	 * @brief Gets the swap chain format.
	 * @return Returns a Vulkan format of the swap chain images.
	 */
	VkFormat GetSwapChainImageFormat();

	/**
	 * @brief Gets the swap chain extent.
	 * @return Returns a Vulkan extent of the swap chain.
	 */
	VkExtent2D GetSwapChainExtent();

	/**
	 * @brief Gets the swap chain images.
	 * @return Returns a vector of swap chain images.
	 */
	std::vector<Image> GetSwapChainImages();

	/**
	 * @brief Gets the swap chain image count.
	 * @return Returns an unsigned integer of the image count.
	 */
	uint32_t GetSwapChainImageCount();

	/**
	 * @brief Gets the frame buuffers.
	 * @return Returns a vector of Vulkan frame buffer handles.
	 */
	std::vector<VkFramebuffer> GetFramebuffers();
};

};