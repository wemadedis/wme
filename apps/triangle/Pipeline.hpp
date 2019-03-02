#pragma once
#include <vulkan/vulkan.h>

namespace Renderer::Pipeline
{
    struct GraphicsPipeline 
    {
        VkPipeline pipeline;
        VkPipelineLayout layout;
    };

    VkPipelineShaderStageCreateInfo GetVertexShaderStageInfo();
    VkPipelineShaderStageCreateInfo GetFragmentShaderStageInfo();
    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo();
    VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyCreateInfo();
    VkPipelineDepthStencilStateCreateInfo GetDepthStencilCreateInfo();
    VkPipelineViewportStateCreateInfo GetViewPortCreateInfo();
    VkPipelineRasterizationStateCreateInfo GetRasterizerCreateInfo();
    VkPipelineMultisampleStateCreateInfo GetMultisamplingCreateInfo();
    VkPipelineColorBlendAttachmentState GetColorBlendAttachment();
    VkPipelineColorBlendStateCreateInfo GetColorBlendCreateInfo();
    VkPipelineLayoutCreateInfo GetPipelineCreateInfo();
    VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout layout);
    VkGraphicsPipelineCreateInfo CreatePipelineCreateInfo(); //Worth it? Would have too many parameters.
    
}