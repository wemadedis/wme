#pragma once
#include <vector>


#include <vulkan/vulkan.h>

#include "Instance.hpp"
#include "ImageManager.hpp"
class SwapChain
{

private:
	Instance* _instance;
	int _framebufferWidth, _framebufferHeight;
	VkSwapchainKHR _swapChain;
	uint32_t _swapChainImageCount;
	std::vector<Image> _swapChainImages;
	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	std::vector<VkImageView> _swapChainImageViews;
	std::vector<VkFramebuffer> _swapChainFramebuffers;
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
	VkSwapchainKHR GetSwapChain();
	VkFormat GetSwapChainImageFormat();
	VkExtent2D GetSwapChainExtent();
	std::vector<Image>& GetSwapChainImages();
	uint32_t GetSwapChainImageCount();
};