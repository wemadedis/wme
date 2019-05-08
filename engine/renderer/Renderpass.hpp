#pragma once
#include <vulkan/vulkan.h>


#include "SwapChain.hpp"

namespace RTE::Rendering
{


class GraphicsPipeline;

class RenderPass 
{

private:

    /**
     * @brief Pointer to the instance.
     */
    Instance *_instance;

    /**
     * @brief Pointer to the swap chain.
     */
    SwapChain *_swapChain;

    /**
     * @brief Vulkan handle to the render pass.
     */
    VkRenderPass _vkrpHandle;

    /**
     * @brief Creates the color attachment for the render pass creation.
     */
    VkAttachmentDescription GetColorAttachment();

    /**
     * @brief Creates the color attachment reference for the render pass creation.
     */
    VkAttachmentReference GetColorAttachmentReference();

    /**
     * @brief Creates the depth attachment for the render pass creation.
     */
    VkAttachmentDescription GetDepthAttachment();

    /**
     * @brief Creates the depth attachment reference for the render pass creation.
     */
    VkAttachmentReference GetDepthAttachmentReference();

    /**
     * @brief Creates a sub pass description.
     */
    VkSubpassDescription GetSubpassDescription(VkAttachmentReference &colorRef, VkAttachmentReference &depthRef);

    /**
     * @brief Creates the vulkan render pass.
     */
    void CreateRenderPass();

public:
    RenderPass(Instance *instance, SwapChain *swapChain);
    ~RenderPass();

    /**
     * @brief Get the render pass handle.
     * @return Returns a Vulkan handle to the render pass.
     */
    VkRenderPass GetHandle();

    /**
     * @brief Begins a render pass for recording of draw commands.
     */
    void BeginRenderPass(VkCommandBuffer cmdBuffer, VkFramebuffer frameBuffer, glm::vec4 clearColor);

    /**
     * @brief Transitions to the next subpass.
     * @param cmdBuffer Command buffer used for recording the commands.
     * @param contents Info on how commands in the next subpass will be provided.
     */
    void NextSubpass(VkCommandBuffer cmdBuffer, VkSubpassContents contents);

    /**
     * @brief Ends a render pass, disabling recording the draw commands to it.
     * @param cmdBuffer Command buffer used for recording the commands.
     */
    void EndRenderPass(VkCommandBuffer cmdBuffer);
};

};