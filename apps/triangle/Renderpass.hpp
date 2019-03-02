#pragma once
#include <vulkan/vulkan.h>


class RenderPass 
{

private:
    VkRenderPass _vkrphandle;
    VkFormat _swapChainImageFormat;
    VkFormat _depthFormat;
    VkDevice _device;

    VkAttachmentDescription GetColorAttachment();
    VkAttachmentReference GetColorAttachmentReference();

    VkAttachmentDescription GetDepthAttachment();
    VkAttachmentReference GetDepthAttachmentReference();

    VkSubpassDescription GetSubpassDescription(VkAttachmentReference &colorRef, VkAttachmentReference &depthRef);
    VkSubpassDependency GetSubpassDependency();

    void CreateRenderPass();

public:
    RenderPass(VkFormat swapChainImageFormat, VkFormat depthFormat, VkDevice device);
    ~RenderPass();
    VkRenderPass GetHandle();
};