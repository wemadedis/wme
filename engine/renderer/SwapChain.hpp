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
	Instance* _instance;
	int _framebufferWidth, _framebufferHeight;
	VkSwapchainKHR _swapChain;
	uint32_t _swapChainImageCount;
	std::vector<ImageInfo> _swapChainImages;
	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	std::vector<VkImageView> _swapChainImageViews;
	std::vector<VkFramebuffer> _swapChainFramebuffers;
	ImageInfo _depthImage;
	VkImageView CreateSwapChainImageView(VkImage image);
	void CreateSwapChainImages();
	void CreateSwapChain();

public:
	struct SupportInformation 
	{
		VkSurfaceCapabilitiesKHR _surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> _sufraceFormats;
		std::vector<VkPresentModeKHR> _presentModes;
	};

    static SupportInformation GetSupportInformation(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	SwapChain(Instance *instance, int framebufferWidth, int frameBufferHeight);
	~SwapChain();
	void CreateFramebuffers(RenderPass *renderPass, ImageManager *imageManager);

	VkSwapchainKHR GetSwapChain();
	VkFormat GetSwapChainImageFormat();
	VkExtent2D GetSwapChainExtent();
	std::vector<ImageInfo>& GetSwapChainImages();
	uint32_t GetSwapChainImageCount();
	std::vector<VkFramebuffer>& GetFramebuffers();
};

};