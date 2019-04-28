#pragma once

#include <vulkan/vulkan.h>

namespace RTE::GUI
{

struct GUIInitInfo
{
    VkInstance          Instance;
    VkPhysicalDevice    PhysicalDevice;
    VkDevice            Device;
    uint32_t            QueueFamily;
    VkQueue             Queue;
    VkPipelineCache     PipelineCache = VK_NULL_HANDLE;
    VkDescriptorPool    DescriptorPool;
    uint32_t            MinImageCount;          // >= 2
    uint32_t            ImageCount;             // >= MinImageCount
    const VkAllocationCallbacks* Allocator;
    void                (*CheckVkResultFn)(VkResult err);
};

class GUIModule
{
    GUIInitInfo _initInfo;
public:
    GUIModule() = default;

    void Initialize(GUIInitInfo info, VkRenderPass rp, VkCommandBuffer cmdBuffer);
    void Draw(VkCommandBuffer cmdBuffer, uint32_t frameWidth, uint32_t frameHeight);
    void SetupInputCallbacks();
    void (*ImGUIDrawCommands)() = nullptr;
};

}