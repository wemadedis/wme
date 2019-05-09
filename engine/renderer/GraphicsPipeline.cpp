#include "GraphicsPipeline.hpp"
#include "Utilities.h"
#include "include/rte/RenderStructs.h"
#include "RTUtilities.h"
namespace RTE::Rendering
{

VkPipelineShaderStageCreateInfo GraphicsPipeline::GetPipelineStageInfo(VkShaderStageFlagBits stage, ShaderInfo shader)
{
    VkPipelineShaderStageCreateInfo stageInfo = {};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = stage;
    stageInfo.module = shader.Module;
    stageInfo.pName = "main";
    return stageInfo;
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

VkPipelineInputAssemblyStateCreateInfo GraphicsPipeline::GetInputAssemblyCreateInfo(VkPrimitiveTopology primitiveTopology)
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = primitiveTopology;
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
    pipelineLayoutInfo.pSetLayouts = _descriptorManager->GetDescriptorLayout(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;		   // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr;	  // Optional

    if (vkCreatePipelineLayout(_instance->GetDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void GraphicsPipeline::CreatePipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader, VkPrimitiveTopology primitiveTopology)
{
    VkPipelineShaderStageCreateInfo vShaderStageInfo = GetPipelineStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader);
    VkPipelineShaderStageCreateInfo fShaderStageInfo = GetPipelineStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader);
    
    VkPipelineShaderStageCreateInfo shaderStages[] = {vShaderStageInfo, fShaderStageInfo};
    auto vAttDesc = Vertex::getAttributeDescriptions();
    auto vBindDesc =  Vertex::getBindingDescription();

    VkPipelineVertexInputStateCreateInfo vInputInfo =  GetVertexInputInfo(vBindDesc, vAttDesc.data(), vAttDesc.size());
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = GetInputAssemblyCreateInfo(primitiveTopology);
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

    if (vkCreateGraphicsPipelines(_instance->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    //After creating the pipeline the modules can be deleted
    //vkDestroyShaderModule(_device, _vertexShaderModule, nullptr);
    //vkDestroyShaderModule(_device, _vertexShaderModule, nullptr);
}


void GraphicsPipeline::CreatePipelineRT(ShaderInfo rayGen, ShaderInfo rchit, ShaderInfo rmiss, ShaderInfo srchit, ShaderInfo srmiss)
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(
    {
        GetPipelineStageInfo(VK_SHADER_STAGE_RAYGEN_BIT_NV, rayGen),
        GetPipelineStageInfo(VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV, rchit),
        GetPipelineStageInfo(VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV, srchit),
        GetPipelineStageInfo(VK_SHADER_STAGE_MISS_BIT_NV, rmiss),
        GetPipelineStageInfo(VK_SHADER_STAGE_MISS_BIT_NV, srmiss)
    });

    auto descriptorLayout = _descriptorManager->GetDescriptorLayoutRT();

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)descriptorLayout.size();
    pipelineLayoutCreateInfo.pSetLayouts = descriptorLayout.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    VkResult code = vkCreatePipelineLayout(_instance->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout);
    Utilities::CheckVkResult(code, "Failed to create RT pipeline layout!");

    std::vector<VkRayTracingShaderGroupCreateInfoNV> shaderGroups({
        // group0 = [ raygen ]
        { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV, nullptr, VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV, 0, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV },
        // group1 = [ chit ]
        { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV, nullptr, VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV, VK_SHADER_UNUSED_NV, 1, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV },
        // group2 = [ srchit ]
        { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV, nullptr, VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_NV, VK_SHADER_UNUSED_NV, 2, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV },
        // group3 = [ miss ]
        { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV, nullptr, VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV, 3, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV },
        // group4 = [ srmiss ]
        { VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV, nullptr, VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_NV, 4, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV, VK_SHADER_UNUSED_NV },
    });

    VkRayTracingPipelineCreateInfoNV rayPipelineInfo;
    rayPipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV;
    rayPipelineInfo.pNext = nullptr;
    rayPipelineInfo.flags = 0;
    rayPipelineInfo.stageCount = (uint32_t)shaderStages.size();
    rayPipelineInfo.pStages = shaderStages.data();
    rayPipelineInfo.groupCount = (uint32_t)shaderGroups.size();
    rayPipelineInfo.pGroups = shaderGroups.data();
    rayPipelineInfo.maxRecursionDepth = 1;
    rayPipelineInfo.layout = _pipelineLayout;
    rayPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    rayPipelineInfo.basePipelineIndex = 0;

    code = RTUtilities::GetInstance()->vkCreateRayTracingPipelinesNV(_instance->GetDevice(), nullptr, 1, &rayPipelineInfo, nullptr, &_pipeline);
    Utilities::CheckVkResult(code, "Failed to create RT pipeline!");

}

GraphicsPipeline::GraphicsPipeline( ShaderInfo vertexShader, 
                                    ShaderInfo fragmentShader, 
                                    VkExtent2D swapChainExtent, 
                                    DescriptorManager *descriptorManager, 
                                    Instance *instance, 
                                    RenderPass* renderPass, 
                                    VkPrimitiveTopology primitiveTopology)
{
    _swapChainExtent = swapChainExtent;
    _descriptorManager = descriptorManager;
    _instance = instance;
    _renderPass = renderPass;
    CreatePipeline(vertexShader, fragmentShader, primitiveTopology);
    
}

GraphicsPipeline::GraphicsPipeline( ShaderInfo rayGen, 
                                    ShaderInfo rchit, 
                                    ShaderInfo rmiss,
                                    ShaderInfo srchit, 
                                    ShaderInfo srmiss,
                                    VkExtent2D swapChainExtent, 
                                    DescriptorManager *descriptorManager, 
                                    Instance *instance, 
                                    RenderPass* renderPass)
{
    _swapChainExtent = swapChainExtent;
    _descriptorManager = descriptorManager;
    _instance = instance;
    _renderPass = renderPass;
    //TODO: Check shader type!
    CreatePipelineRT(rayGen, rchit, rmiss, srchit, srmiss);
}

GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(_instance->GetDevice(), _pipeline, nullptr);
    vkDestroyPipelineLayout(_instance->GetDevice(), _pipelineLayout, nullptr);
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