#pragma once
#include <vulkan/vulkan.h>
#include "Renderpass.hpp"

namespace RTE::Renderer
{

class GraphicsPipeline
{
    
private:
    VkExtent2D _swapChainExtent;
    VkDescriptorSetLayout _descriptorSetLayout;
    VkDevice _device;
    RenderPass* _renderPass;
    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;

    VkPipelineShaderStageCreateInfo GetVertexShaderStageInfo(VkShaderModule vertexModule);
    VkPipelineShaderStageCreateInfo GetFragmentShaderStageInfo(VkShaderModule fragmentModule);
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
    void CreatePipelineLayout();
    void CreatePipeline(VkShaderModule vertexShaderModule, VkShaderModule fragmentShaderModule); //Worth it? Would have too many parameters.
public:
    GraphicsPipeline(const char* vertexShaderPath, const char* fragShaderPath, VkExtent2D swapChainExtent, VkDescriptorSetLayout layout, VkDevice device, RenderPass* renderPass);
    ~GraphicsPipeline();
    VkPipeline GetHandle();
    VkPipelineLayout GetLayout();
};

};