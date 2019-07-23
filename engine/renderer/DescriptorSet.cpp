#include "DescriptorSet.h"
#include "Utilities.h"

namespace RTE::Rendering
{

typedef DescriptorSet::DescriptorSetBuilder DescriptorSetBuilder;

void DescriptorSetBuilder::CreateDescriptorSetLayout(LayoutInfo& layoutInfo)
{

}

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

DescriptorSet* DescriptorSetBuilder::Build(Instance* instance)
{
    
    std::vector<LayoutInfo>* layoutInfos = {};
    //Push an empty LayoutInfo, used for all descriptors of count 1
    layoutInfos->push_back({});

    //Add bindings to different "bins" (LayoutInfos).
    std::map<std::string, VkDescriptorSetLayoutBinding>::iterator bindingIterator;
    for(bindingIterator = _layoutBindings.begin(); bindingIterator != _layoutBindings.end(); bindingIterator++)
    {
        std::string name = bindingIterator->first;
        VkDescriptorSetLayoutBinding binding = bindingIterator->second;
        if(binding.descriptorCount == 1)
        {
            layoutInfos->at(0).Bindings.insert({name, binding});
        }
        else
        {
            LayoutInfo variableSizeBindingLayoutInfo = {};
            variableSizeBindingLayoutInfo.HasVariableSizeBinding = true;
            variableSizeBindingLayoutInfo.Bindings.insert({name, binding});
            layoutInfos->push_back(variableSizeBindingLayoutInfo);
        }
    }

    for(LayoutInfo layoutInfo : *layoutInfos)
    {
        uint32_t bindingsCount = static_cast<uint32_t>(layoutInfo.Bindings.size()); 
        
        if(!layoutInfo.HasVariableSizeBinding)
        {
            //If no bindings with descriptor count 1 specified, continue (Would it ever be the case though? Maybe throw exception?)
            if(bindingsCount < 1) continue;
            //Need to put bindings into contiguous memory.
            std::vector<VkDescriptorSetLayoutBinding> bindings = {};
            for(auto bindingPair : layoutInfo.Bindings)
            {
                bindings.push_back(bindingPair.second);
            }

            //Create the layout
            VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutCreateInfo.pBindings = bindings.data();

            VkResult code = vkCreateDescriptorSetLayout(instance->GetDevice(), &layoutCreateInfo, nullptr, &layoutInfo.Layout);
            Utilities::CheckVkResult(code, "DescriptorManager: Failed to create descriptor set layout!");
        }
        else
        {
            //Something goes wrong in the processing of the bindings if this is the case
            if(bindingsCount < 1 || bindingsCount > 1) throw "Exception";
            
            VkDescriptorBindingFlagsEXT flag = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT;
            VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlags;
            bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
            bindingFlags.pNext = nullptr;
            bindingFlags.pBindingFlags = &flag;
            bindingFlags.bindingCount = 1; //Layouts with variable sized descriptors has only that one binding

            VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
            layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutCreateInfo.pNext = &bindingFlags;
            layoutCreateInfo.flags = 0;
            layoutCreateInfo.bindingCount = 1;
            //Can safely assume that there is only one layout binding struct.
            layoutCreateInfo.pBindings = &layoutInfo.Bindings[0];
            VkResult code = vkCreateDescriptorSetLayout(instance->GetDevice(), &layoutCreateInfo, nullptr, &layoutInfo.Layout);
        }
    }

    return new DescriptorSet(layoutInfos);
}

uint32_t DescriptorSet::Allocate()
{
    return 0;
}

};