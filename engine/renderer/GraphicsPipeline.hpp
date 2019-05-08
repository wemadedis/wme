#pragma once
#include <vulkan/vulkan.h>
#include "Renderpass.hpp"
#include "DescriptorManager.hpp"

namespace RTE::Rendering
{


/**
 * @brief
 */
class GraphicsPipeline
{
    
private:

    /**
     * @brief Pointer to the instance.
     */
    Instance *_instance;
    
    /**
     * @brief Pointer to the descriptor manager.
     */
    DescriptorManager *_descriptorManager;
    
    /**
     * @brief Pointer to the render pass.
     */
    RenderPass* _renderPass;
    
    /**
     * @brief The swap chain extent.
     */
    VkExtent2D _swapChainExtent;
    
    /**
     * @brief Vulkan handle to the pipeline layout.
     */
    VkPipelineLayout _pipelineLayout;
    
    /**
     * @brief Vulkan handle to the pipeline.
     */
    VkPipeline _pipeline;
    
    /**
     * @brief Creates a pipeline shader stage info.
     */
    VkPipelineShaderStageCreateInfo GetPipelineStageInfo(VkShaderStageFlagBits stage, ShaderInfo shader);
    
    /**
     * @brief Creates the pipeline vertex input info.
     */
    VkPipelineVertexInputStateCreateInfo GetVertexInputInfo(VkVertexInputBindingDescription &binding, VkVertexInputAttributeDescription *attribute, uint32_t attributeDescriptionCount);
    
    /**
     * @brief Creates the pipeline input assembly info for triangle primitives.
     */
    VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyCreateInfo();
    
    /**
     * @brief Creates the pipeline input assembly info for line primitives.
     */
    VkPipelineInputAssemblyStateCreateInfo GetLineInputAssemblyCreateInfo();
    
    /**
     * @brief Creates the pipeline depth stencil info.
     */
    VkPipelineDepthStencilStateCreateInfo GetDepthStencilCreateInfo();
    
    /**
     * @brief Defines the viewport used for the pipeline.
     */
    VkViewport GetViewport();
    
    /**
     * @brief Defines the scissor used for the pipeline.
     */
    VkRect2D GetScissor();
    
    /**
     * @brief Creates the pipeline viewport info.
     */
    VkPipelineViewportStateCreateInfo GetViewPortCreateInfo(VkViewport &viewport, VkRect2D &scissor);
    
    /**
     * @brief Creates the pipeline rasterization info.
     */
    VkPipelineRasterizationStateCreateInfo GetRasterizerCreateInfo();
    
    /**
     * @brief Creates the pipeline multisample info.
     */
    VkPipelineMultisampleStateCreateInfo GetMultisampleCreateInfo();
    
    /**
     * @brief
     */
    VkPipelineColorBlendAttachmentState GetColorBlendAttachment();
    
    /**
     * @brief
     */
    VkPipelineColorBlendStateCreateInfo GetColorBlendCreateInfo(VkPipelineColorBlendAttachmentState &colorBlendAttachment);
    
    /**
     * @brief
     */
    void CreatePipelineLayout();
    
    /**
     * @brief
     */
    void CreatePipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader);
    
    /**
     * @brief
     */
    void CreateLinePipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader);
    
    /**
     * @brief
     */
    void CreatePipelineRT(ShaderInfo rayGen, ShaderInfo rchit, ShaderInfo rmiss, ShaderInfo srchit, ShaderInfo srmiss);
public:
    GraphicsPipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader, 
                     VkExtent2D swapChainExtent, DescriptorManager *descriptorManager, 
                    Instance *instance, RenderPass* renderPass);

    GraphicsPipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader, 
                    VkExtent2D swapChainExtent, DescriptorManager *descriptorManager, 
                    Instance *instance, RenderPass* renderPass, bool lines);

    GraphicsPipeline(ShaderInfo rayGen, 
                     ShaderInfo rchit, 
                     ShaderInfo rmiss, 
                     ShaderInfo srchit, 
                     ShaderInfo srmiss,
                     VkExtent2D swapChainExtent, 
                     DescriptorManager *descriptorManager, 
                     Instance *instance, 
                     RenderPass* renderPass);
    ~GraphicsPipeline();
    VkPipeline GetHandle();
    VkPipelineLayout GetLayout();
};

};