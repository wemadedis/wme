#include "DescriptorSet.h"

namespace RTE::Rendering
{

typedef DescriptorSet::DescriptorSetBuilder DescriptorSetBuilder;

DescriptorSetBuilder& DescriptorSetBuilder::AddLayoutBinding(std::string name, VkDescriptorType descriptorType, uint32_t descriptorCount, VkShaderStageFlags shaderStages)
{
    uint32_t binding = _currentBinding;
    if(descriptorCount < 1)
    {
        throw "Exception";
    }
    VkDescriptorSetLayoutBinding layoutBinding = {};
    //If the descriptor count is larger than 1, set binding to 0, otherwise set to current binding count and increment it.
    layoutBinding.binding = (descriptorCount > 1) ? 0 : _currentBinding++;
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = descriptorCount;
    layoutBinding.stageFlags = shaderStages;
    layoutBinding.pImmutableSamplers = nullptr;

    _layoutBindings.insert({name, layoutBinding});
    return *this;
}


DescriptorSetBuilder& DescriptorSetBuilder::WithUniformBuffer(std::string name,
                                                uint32_t descriptorCount,
                                                VkShaderStageFlags shaderStages)
{
    return AddLayoutBinding(name, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount, shaderStages);
}

DescriptorSetBuilder& DescriptorSetBuilder::WithUniformTexelBuffer(std::string name,
                                                uint32_t descriptorCount,
                                                VkShaderStageFlags shaderStages)
{
    return AddLayoutBinding(name, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, descriptorCount, shaderStages);
}
DescriptorSetBuilder& DescriptorSetBuilder::WithCombinedImageSampler(std::string name,
                                                uint32_t descriptorCount,
                                                VkShaderStageFlags shaderStages)
{
    return AddLayoutBinding(name, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount, shaderStages);
}

DescriptorSetBuilder& DescriptorSetBuilder::WithStorageImage(std::string name,
                                        uint32_t descriptorCount,
                                        VkShaderStageFlags shaderStages)         
{
    return AddLayoutBinding(name, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptorCount, shaderStages);
}

DescriptorSetBuilder& DescriptorSetBuilder::WithAccelerationStructure(std::string name,
                                                uint32_t descriptorCount,
                                                VkShaderStageFlags shaderStages)
{
    return AddLayoutBinding(name, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, descriptorCount, shaderStages);
}

DescriptorSet* DescriptorSetBuilder::Build()
{
    
    return new DescriptorSet({});
}

uint32_t DescriptorSet::Allocate()
{

}

};