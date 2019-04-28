#pragma once

#include <vulkan/vulkan.h>
#include <functional>

namespace RTE::GUI
{
typedef std::function <void()> DrawFunc;
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
    DrawFunc DrawFunction = nullptr;
};

}