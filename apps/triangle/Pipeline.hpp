#pragma once
#include <vulkan/vulkan.h>

namespace Renderer::Pipeline
{
    struct GraphicsPipeline 
    {
        VkPipeline pipeline;
        VkPipelineLayout layout;
    };

    
}