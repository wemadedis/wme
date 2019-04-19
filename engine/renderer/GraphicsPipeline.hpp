#pragma once
#include <vulkan/vulkan.h>
#include "Renderpass.hpp"
#include "DescriptorManager.hpp"

namespace RTE::Rendering
{

class GraphicsPipeline
{
    
private:
    VkExtent2D _swapChainExtent;
    DescriptorManager *_descriptorManager;
    Instance *_instance;
    RenderPass* _renderPass;
    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;


    VkPipelineShaderStageCreateInfo GetPipelineStageInfo(VkShaderStageFlagBits stage, ShaderInfo shader);
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
    void CreatePipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader);

    
    void CreatePipelineRT(ShaderInfo rayGen, ShaderInfo rchit, ShaderInfo rmiss);
public:
    GraphicsPipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader, VkExtent2D swapChainExtent, DescriptorManager *descriptorManager, Instance *instance, RenderPass* renderPass);
    GraphicsPipeline(ShaderInfo rayGen, ShaderInfo rchit, ShaderInfo rmiss, VkExtent2D swapChainExtent, DescriptorManager *descriptorManager, Instance *instance, RenderPass* renderPass);
    ~GraphicsPipeline();
    VkPipeline GetHandle();
    VkPipelineLayout GetLayout();
};

};