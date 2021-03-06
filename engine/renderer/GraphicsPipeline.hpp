#pragma once
#include <vulkan/vulkan.h>
#include "Renderpass.hpp"
#include "DescriptorManager.hpp"

namespace RTE::Rendering
{


/**
 * @brief Encapsulates the relevant objects and the process of creating a Vulkan pipeline for both rasterization and ray tracing.
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
     * @param primitiveTopology The primitive topology used by the pipeline.
     */
    VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyCreateInfo(VkPrimitiveTopology primitiveTopology);
    
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
     * @brief Defines the color blend attachment create info.
     */
    VkPipelineColorBlendStateCreateInfo GetColorBlendCreateInfo(VkPipelineColorBlendAttachmentState &colorBlendAttachment);
    
    /**
     * @brief Creates the color blend attachment.
     */
    VkPipelineColorBlendAttachmentState GetColorBlendAttachment();

    /**
     * @brief Creates the pipeline layout.
     */
    void CreatePipelineLayout();
    
    /**
     * @brief Creates the triangle rasterization pipeline.
     * @param vertexShader The vertex shader info.
     * @param fragmentShader The fragment shader info.
     */
    void CreatePipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader, VkPrimitiveTopology primitiveTopology);

    /**
     * @brief Creates the ray tracing pipeline.
     * @param rayGen The ray generation shader info.
     * @param rchit The closest hit shader info.
     * @param rmiss The ray miss shader info.
     * @param srchit The shadow ray closest hit shader info.
     * @param srmiss The shadow ray miss shader info.
     */
    void CreatePipelineRT(ShaderInfo rayGen, ShaderInfo rchit, ShaderInfo rmiss, ShaderInfo srchit, ShaderInfo srmiss);
public:

    /**
     * @brief Contructor for the rasterization pipeline.
     * @param vertexShader The vertex shader info.
     * @param fragmentShader The fragment shader info.
     * @param swapChainExtent The swap chain extent.
     * @param descriptorManager Pointer to the descriptor manager.
     * @param instance Pointer to the instance.
     * @param renderPass Pointer to the render pass.
     * @param primitiveTopology The primitive topology used for rendering.
     */
    GraphicsPipeline(ShaderInfo vertexShader, ShaderInfo fragmentShader, 
                     VkExtent2D swapChainExtent, DescriptorManager *descriptorManager, 
                    Instance *instance, RenderPass* renderPass, VkPrimitiveTopology primitiveTopology);

    /**
     * @brief Contructor for the ray tracing pipeline.
     * @param rayGen The ray generation shader info.
     * @param rchit The ray closest hit shader info.
     * @param rmiss The ray miss shader info.
     * @param srchit The shadow ray closest hit shader info.
     * @param srmiss The shadow ray miss shader info.
     * @param descriptorManager Pointer to the descriptor manager.
     * @param instance Pointer to the instance.
     * @param renderPass Pointer to the render pass.
     */
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

    /**
     * @brief Gets the pipeline handle.
     * @return Returns a Vulkan handle to the pipeline.
     */
    VkPipeline GetHandle();

    /**
     * @brief Gets the pipeline layout handle.
     * @return Returns a Vulkan handle to the pipeline layout.
     */
    VkPipelineLayout GetLayout();
};

};