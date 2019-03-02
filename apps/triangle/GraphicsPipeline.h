#pragma once
#include <vulkan/vulkan.h>

class GraphicsPipeline
{
    
private:
    VkShaderModule _vertexShaderModule, _fragShaderModule;
    VkExtent2D _swapChainExtent;
    VkDescriptorSetLayout _descriptorSetLayout;
    VkDevice _device;
    VkRenderPass _renderPass;
    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;

    VkPipelineShaderStageCreateInfo GetVertexShaderStageInfo();
    VkPipelineShaderStageCreateInfo GetFragmentShaderStageInfo();
    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo(VkVertexInputBindingDescription &binding, VkVertexInputAttributeDescription *attribute, uint32_t attributeDescriptionCount);
    VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyCreateInfo();
    VkPipelineDepthStencilStateCreateInfo GetDepthStencilCreateInfo();
    VkViewport GetViewport();
    VkRect2D GetScissor();
    VkPipelineViewportStateCreateInfo GetViewPortCreateInfo(VkViewport &viewport, VkRect2D &scissor);
    VkPipelineRasterizationStateCreateInfo GetRasterizerCreateInfo();
    VkPipelineMultisampleStateCreateInfo GetMultisampleCreateInfo();
    VkPipelineColorBlendAttachmentState GetColorBlendAttachment();
    VkPipelineColorBlendStateCreateInfo GetColorBlendCreateInfo(VkPipelineColorBlendAttachmentState &colorBlendAttachment);
    VkPipelineLayout CreatePipelineLayout();
    void CreatePipeline(); //Worth it? Would have too many parameters.
public:
    GraphicsPipeline(VkShaderModule vertexShaderModule, VkShaderModule fragShaderModule, VkExtent2D swapChainExtent, VkDescriptorSetLayout layout, VkDevice device, VkRenderPass renderPass);
    ~GraphicsPipeline();
    VkPipeline GetHandle();
    VkPipelineLayout GetLayout();
};