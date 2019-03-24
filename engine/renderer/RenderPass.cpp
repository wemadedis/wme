#include "Renderpass.hpp"
#include <array>

#include "GraphicsPipeline.hpp"

namespace RTE::Rendering
{

VkAttachmentDescription RenderPass::GetColorAttachment()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = _swapChain->GetSwapChainImageFormat();
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
    depthAttachment.format = _instance->GetOptimalDepthFormat();
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

    if (vkCreateRenderPass(_instance->GetDevice(), &renderPassInfo, nullptr, &_vkrpHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

RenderPass::RenderPass(Instance *instance, SwapChain *swapChain)
{
    _instance = instance;
    _swapChain = swapChain;
    CreateRenderPass();
}

RenderPass::~RenderPass()
{
    vkDestroyRenderPass(_instance->GetDevice(), _vkrpHandle, nullptr);
}

VkRenderPass RenderPass::GetHandle()
{
    return _vkrpHandle;
}

void RenderPass::BeginRenderPass(GraphicsPipeline *pipeline, VkCommandBuffer cmdBuffer, VkFramebuffer frameBuffer)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("RenderPass: Failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _vkrpHandle;
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapChain->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    
    vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetHandle());
}

void RenderPass::EndRenderPass(VkCommandBuffer cmdBuffer)
{
    vkCmdEndRenderPass(cmdBuffer);
    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("RenderPass: Failed to record command buffer!");
    }
}

};