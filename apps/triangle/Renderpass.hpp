#pragma once
#include <vulkan/vulkan.h>


#include "SwapChain.hpp"

class GraphicsPipeline;

class RenderPass 
{

private:
    VkRenderPass _vkrpHandle;
    Instance *_instance;
    SwapChain *_swapChain;

    VkAttachmentDescription GetColorAttachment();
    VkAttachmentReference GetColorAttachmentReference();

    VkAttachmentDescription GetDepthAttachment();
    VkAttachmentReference GetDepthAttachmentReference();

    VkSubpassDescription GetSubpassDescription(VkAttachmentReference &colorRef, VkAttachmentReference &depthRef);
    VkSubpassDependency GetSubpassDependency();

    void CreateRenderPass();

public:
    RenderPass(Instance *instance, SwapChain *swapChain);
    ~RenderPass();
    VkRenderPass GetHandle();
    void BeginRenderPass(GraphicsPipeline *pipeline, VkCommandBuffer *cmdbuffer);
};