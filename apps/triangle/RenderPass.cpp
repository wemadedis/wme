#include "Renderpass.hpp"
#include <array>

VkAttachmentDescription RenderPass::GetColorAttachment()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = _swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    return colorAttachment;
}

VkAttachmentReference RenderPass::GetColorAttachmentReference()
{
    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    return colorAttachmentReference;
}

VkAttachmentDescription RenderPass::GetDepthAttachment()
{
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = _depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    return depthAttachment;
}

VkAttachmentReference RenderPass::GetDepthAttachmentReference()
{
    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    return depthAttachmentReference;
}

VkSubpassDescription RenderPass::GetSubpassDescription(VkAttachmentReference &colorRef, VkAttachmentReference &depthRef)
{
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;
    return subpass;
}

VkSubpassDependency RenderPass::GetSubpassDependency()
{
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    return dependency;
}

void RenderPass::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = GetColorAttachment();
    VkAttachmentDescription depthAttachment = GetDepthAttachment();
    
    VkAttachmentReference colorAttachmentReference = GetColorAttachmentReference();
    VkAttachmentReference depthAttachmentReference = GetDepthAttachmentReference();
    VkSubpassDescription subpassDescription = GetSubpassDescription(colorAttachmentReference, depthAttachmentReference);
    
    VkSubpassDependency subpassDependency = GetSubpassDependency();
    
    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &subpassDependency;

    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_vkrphandle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

RenderPass::RenderPass(VkFormat swapChainImageFormat, VkFormat depthFormat, VkDevice device)
{
    _swapChainImageFormat = swapChainImageFormat;
    _depthFormat = depthFormat;
    _device = device;
    CreateRenderPass();
}

RenderPass::~RenderPass()
{
    vkDestroyRenderPass(_device, _vkrphandle, nullptr);
}

VkRenderPass RenderPass::GetHandle()
{
    return _vkrphandle;
}