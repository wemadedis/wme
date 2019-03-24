#include "GraphicsPipeline.hpp"
#include "Utilities.h"
#include "include/rte/RenderStructs.h"
namespace RTE::Rendering
{

VkPipelineShaderStageCreateInfo GraphicsPipeline::GetVertexShaderStageInfo(VkShaderModule vertexModule)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexModule;
    vertShaderStageInfo.pName = "main";
    return vertShaderStageInfo;
}

VkPipelineShaderStageCreateInfo GraphicsPipeline::GetFragmentShaderStageInfo(VkShaderModule fragmentModule)
{
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragmentModule;
    fragShaderStageInfo.pName = "main";
    return fragShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo GraphicsPipeline::GetVertexInputInfo(VkVertexInputBindingDescription &binding, VkVertexInputAttributeDescription *attribute, uint32_t attributeDescriptionCount)
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &binding;
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptionCount;
    vertexInputInfo.pVertexAttributeDescriptions = attribute;
    return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo GraphicsPipeline::GetInputAssemblyCreateInfo()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

VkPipelineDepthStencilStateCreateInfo GraphicsPipeline::GetDepthStencilCreateInfo()
{
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional
    return depthStencil;
}

VkViewport GraphicsPipeline::GetViewport()
{
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_swapChainExtent.width;
    viewport.height = (float)_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

VkRect2D GraphicsPipeline::GetScissor()
{
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = _swapChainExtent;
    return scissor;
}

VkPipelineViewportStateCreateInfo GraphicsPipeline::GetViewPortCreateInfo(VkViewport &viewport, VkRect2D &scissor)
{
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    return viewportState;
}

VkPipelineRasterizationStateCreateInfo GraphicsPipeline::GetRasterizerCreateInfo()
{
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f; //if fill mode is not used.
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;		   // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;	// Optional
    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo GraphicsPipeline::GetMultisampleCreateInfo()
{
    //Currently disabled?
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;			// Optional
    multisampling.pSampleMask = nullptr;			// Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;		// Optional
    return multisampling;
}

VkPipelineColorBlendAttachmentState GraphicsPipeline::GetColorBlendAttachment()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;			 // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;			 // Optional
    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo GraphicsPipeline::GetColorBlendCreateInfo(VkPipelineColorBlendAttachmentState &colorBlendAttachment)
{
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional
    return colorBlending;
}

void GraphicsPipeline::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;				   // Optional
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;		   // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr;	  // Optional

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void GraphicsPipeline::CreatePipeline(VkShaderModule vertexShaderModule, VkShaderModule fragmentShaderModule)
{
    VkPipelineShaderStageCreateInfo vShaderStageInfo = GetVertexShaderStageInfo(vertexShaderModule);
    VkPipelineShaderStageCreateInfo fShaderStageInfo = GetFragmentShaderStageInfo(fragmentShaderModule);
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {vShaderStageInfo, fShaderStageInfo};
    auto vAttDesc = Vertex::getAttributeDescriptions();
    auto vBindDesc =  Vertex::getBindingDescription();

    VkPipelineVertexInputStateCreateInfo vInputInfo =  GetVertexInputInfo(vBindDesc, vAttDesc.data(), vAttDesc.size());
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = GetInputAssemblyCreateInfo();
    VkViewport viewport = GetViewport();
    VkRect2D scissor = GetScissor();
    VkPipelineViewportStateCreateInfo viewportState = GetViewPortCreateInfo(viewport, scissor);
    VkPipelineRasterizationStateCreateInfo rasterizer = GetRasterizerCreateInfo();
    VkPipelineMultisampleStateCreateInfo multisampleInfo = GetMultisampleCreateInfo();
    VkPipelineDepthStencilStateCreateInfo depthStencil = GetDepthStencilCreateInfo();
    VkPipelineColorBlendAttachmentState colBlendAttachment = GetColorBlendAttachment();
    VkPipelineColorBlendStateCreateInfo colorBlend = GetColorBlendCreateInfo(colBlendAttachment);


    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampleInfo;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlend;
    //pipelineInfo.pDynamicState = nullptr; // Optional
    CreatePipelineLayout();
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass->GetHandle();
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;			  // Optional

    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    //After creating the pipeline the modules can be deleted
    //vkDestroyShaderModule(_device, _vertexShaderModule, nullptr);
    //vkDestroyShaderModule(_device, _vertexShaderModule, nullptr);

}

GraphicsPipeline::GraphicsPipeline( const char*  vertexShaderPath, 
                                    const char* fragShaderPath, 
                                    VkExtent2D swapChainExtent, 
                                    VkDescriptorSetLayout layout, 
                                    VkDevice device, 
                                    RenderPass* renderPass) 
{
    _swapChainExtent = swapChainExtent;
    _descriptorSetLayout = layout;
    _device = device;
    _renderPass = renderPass;

    VkShaderModule vertexShaderModule = Utilities::CreateShaderModule(Utilities::ReadEngineAsset(vertexShaderPath), device);
    VkShaderModule fragShaderModule = Utilities::CreateShaderModule(Utilities::ReadEngineAsset(fragShaderPath), device);

    CreatePipeline(vertexShaderModule, fragShaderModule);
    vkDestroyShaderModule(device, vertexShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(_device, _pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
}

VkPipeline GraphicsPipeline::GetHandle()
{
    return _pipeline;
}

VkPipelineLayout GraphicsPipeline::GetLayout()
{
    return _pipelineLayout;
}

};