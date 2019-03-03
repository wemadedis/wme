#pragma once
#include <vector>


#include <vulkan/vulkan.h>

class SwapChain
{
private:
    VkSurfaceCapabilitiesKHR _capabilities;
	std::vector<VkSurfaceFormatKHR> _sufraceFormats;
	std::vector<VkPresentModeKHR> _presentModes;

public:
    
};