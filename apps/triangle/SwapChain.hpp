#pragma once
#include <vector>


#include <vulkan/vulkan.h>

#include "Instance.hpp"

class SwapChain
{

private:
	Instance* _instance;
	int _framebufferWidth, _framebufferHeight;
	VkSwapchainKHR _swapChain;
	std::vector<VkImage> _swapChainImages;
	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	std::vector<VkImageView> _swapChainImageViews;
	std::vector<VkFramebuffer> _swapChainFramebuffers;

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

	VkSwapchainKHR GetSwapChain();
	VkFormat GetSwapChainImageFormat();
	VkExtent2D GetSwapChainExtent();
};