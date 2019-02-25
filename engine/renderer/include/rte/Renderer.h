#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace RTE::Renderer
{

void CreateInstance(VkInstance *instance, std::vector<const char *> extensions, bool enableValidationLayers);
void SetupDebugCallback(VkInstance instance);
void CreateSurface(VkInstance instance);
}