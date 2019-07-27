#include "DescriptorSet.h"
#include "Utilities.h"

namespace RTE::Rendering
{

typedef DescriptorSet::DescriptorSetBuilder DescriptorSetBuilder;

void DescriptorSetBuilder::CreateDescriptorSetLayout(LayoutInfo& layoutInfo, Instance* instance)
{
    uint32_t bindingsCount = static_cast<uint32_t>(layoutInfo.Bindings.size()); 
    
    if(!layoutInfo.HasVariableSizeBinding)
    {
        //If no bindings with descriptor count 1 specified, continue (Would it ever be the case though? Maybe throw exception?)
        if(bindingsCount < 1) {
            //Do something
            throw "Exception";
            return;
        }
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

DescriptorSet* DescriptorSetBuilder::Build(Instance* instance, uint32_t maxSetsPerPool)
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
        CreateDescriptorSetLayout(layoutInfo, instance);
    }

    return new DescriptorSet(layoutInfos, instance, maxSetsPerPool);
}


DescriptorSet::DescriptorSet(std::vector<LayoutInfo>* layoutInfos, Instance* instance, uint32_t maxSets)
{
    _layoutInfos = layoutInfos;
    _instance = instance;
    _maxSetsPerPool = static_cast<uint32_t>(maxSets * layoutInfos->size());
    for(LayoutInfo info : *_layoutInfos)
    {
        _setLayouts.push_back(info.Layout);
        for(auto nameMapping : info.Bindings)
        {
            //Map binding name to the binding and layout it belongs to. Binding to keep track of type and layout for updating the data.
            _bindingMap.insert({nameMapping.first, {nameMapping.second, info.Layout, _descriptorsCount}});
            _descriptorsCount++;
        }
    }
}

void DescriptorSet::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.setLayoutCount = (uint32_t)_setLayouts.size();
    pipelineLayoutInfo.pSetLayouts = _setLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    VkResult code = vkCreatePipelineLayout(_instance->GetDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout);
    Utilities::CheckVkResult(code, "Failed to create pipeline layout!");
}


void DescriptorSet::CreateDescriptorPool()
{
    //If the pool sizes have not been determined yet 
    //(doing this to be extendeble when the max amount of sets have been allocated from the created pool so that a new one can be added)
    if(_poolSizes.size() == 0)
    {
        //Get all pool sizes (one per descriptor type)
        for(auto binding : _bindingMap)
        {
            //                          Hella ugly
            auto bindingInfo = binding.second.Binding;
            auto bindingType = bindingInfo.descriptorType;
            auto poolSize = std::find_if(_poolSizes.begin(), _poolSizes.end(), 
                                        [bindingType](VkDescriptorPoolSize ps) { return ps.type == bindingType; });
            
            if(poolSize != _poolSizes.end()) 
                poolSize->descriptorCount += bindingInfo.descriptorCount;
            else 
                _poolSizes.push_back({bindingType, bindingInfo.descriptorCount});
        }
    }
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = _maxSetsPerPool;
    descriptorPoolCreateInfo.poolSizeCount = (uint32_t)_poolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = _poolSizes.data();

    VkDescriptorPool pool;
    VkResult code = vkCreateDescriptorPool(_instance->GetDevice(), &descriptorPoolCreateInfo, nullptr, &pool);
    Utilities::CheckVkResult(code, "Failed to create a descriptor pool!");
    //If all is fine, add the new pool to the pool collection
    _descriptorPools.push_back(pool);
}

void DescriptorSet::CreateSetAllocationInfo()
{
    for(auto layoutinfo : *_layoutInfos)
    {
        //Can safely take the first binding descriptor count since:
        // - The first layout only contains descriptors of count 1
        // - The remaining layouts contain one descriptor each, with the decriptor count specified.
        uint32_t descriptorCount = layoutinfo.Bindings.begin()->second.descriptorCount;
        _allocationInfo.VariableDescriptorCounts.push_back(descriptorCount);
    }
    //VariableDescriptorCountInfo is used to inform how many descriptors per layout in a descriptor set are used.
    _allocationInfo.VariableDescriptorCountInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    _allocationInfo.VariableDescriptorCountInfo.pNext = nullptr;
    _allocationInfo.VariableDescriptorCountInfo.descriptorSetCount = (uint32_t)_allocationInfo.VariableDescriptorCounts.size();
    _allocationInfo.VariableDescriptorCountInfo.pDescriptorCounts = _allocationInfo.VariableDescriptorCounts.data();
    
    _allocationInfo.DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    _allocationInfo.DescriptorSetAllocateInfo.pNext = &_allocationInfo.VariableDescriptorCountInfo;
    _allocationInfo.DescriptorSetAllocateInfo.descriptorPool = _descriptorPools[_currentPoolIndex];
    _allocationInfo.DescriptorSetAllocateInfo.descriptorSetCount = (uint32_t)_setLayouts.size();
    _allocationInfo.DescriptorSetAllocateInfo.pSetLayouts = _setLayouts.data();
}


VkWriteDescriptorSet DescriptorSet::GetDescriptorWrite(std::string descriptorName, VkDescriptorSet set)
{
    VkDescriptorSetLayoutBinding binding = _bindingMap[descriptorName].Binding;
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.pNext = nullptr;
    descriptorWrite.dstSet = set;
    descriptorWrite.dstBinding = binding.binding;
    descriptorWrite.dstArrayElement = 0; //Wut is dis?
    descriptorWrite.descriptorCount = binding.descriptorCount;
    descriptorWrite.descriptorType = binding.descriptorType;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;
    return descriptorWrite;
}


VkPipelineLayout DescriptorSet::GetPipelineLayout()
{
    return _pipelineLayout;
}

SetInstanceHandle DescriptorSet::Allocate()
{
    VkDescriptorSet descriptorSet;
    VkResult code = vkAllocateDescriptorSets(_instance->GetDevice(), &_allocationInfo.DescriptorSetAllocateInfo, &descriptorSet);
    Utilities::CheckVkResult(code, "Failed to allocate a descriptor set!");
    
    SetInstance instance = {};
    instance.Set = descriptorSet;
    instance.SetWrites = new VkWriteDescriptorSet[_descriptorsCount];
    
    int bindingIndex = 0;
    for(auto bindingMapping : _bindingMap)
        instance.SetWrites[bindingIndex++] = GetDescriptorWrite(bindingMapping.first, instance.Set);
    
    _instances.insert({_setAllocationsCount, instance});

    _poolAllocationCount += (uint32_t)_setLayouts.size();
    
    return static_cast<SetInstanceHandle>(_setAllocationsCount++);
}

void DescriptorSet::UpdateUniformBuffer(SetInstanceHandle handle, std::string descriptorName, BufferInformation *bufferInfos, uint32_t bufferCount)
{
    auto descriptorInfo = _bindingMap[descriptorName];
    VkDescriptorBufferInfo* descriptorBufferInfos = nullptr;
    VkBufferView* bufferViews = nullptr;
    
    
    if(descriptorInfo.Binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) 
    {
        descriptorBufferInfos = new VkDescriptorBufferInfo[bufferCount];
        for(uint32_t bufferIndex = 0; bufferIndex < bufferCount; bufferIndex++)
        {
            descriptorBufferInfos[bufferIndex] = bufferInfos[bufferIndex].descriptorBufferInfo;
        }
        
    }
    else if (descriptorInfo.Binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)
    {
        bufferViews = new VkBufferView[bufferCount];
        for(uint32_t bufferIndex = 0; bufferIndex < bufferCount; bufferIndex++)
        {
            bufferViews[bufferIndex] = bufferInfos[bufferIndex].BufferView; 
        }
    }
    
    _instances[handle].SetWrites[descriptorInfo.BindingIndex].pBufferInfo = descriptorBufferInfos;
    _instances[handle].SetWrites[descriptorInfo.BindingIndex].pTexelBufferView = bufferViews;
}

void DescriptorSet::UpdateImage(SetInstanceHandle handle, std::string descriptorName, ImageInfo *imageInfos, uint32_t imageCount)
{
    auto descriptorInfo = _bindingMap[descriptorName];
    VkDescriptorImageInfo* descriptorImageInfos = new VkDescriptorImageInfo[imageCount];

    for(uint32_t imageIndex = 0; imageIndex < imageCount; imageIndex++)
    {
        descriptorImageInfos[imageIndex] = imageInfos[imageIndex].descriptorImageInfo;
    }
    _instances[handle].SetWrites[descriptorInfo.BindingIndex].pImageInfo = descriptorImageInfos;
}
void DescriptorSet::UpdateAccelerationStructure(SetInstanceHandle handle, std::string descriptorName, AccelerationStructure* as)
{
    auto& ASdescriptorWrite = as->GetDescriptorWrite();
    auto descriptorInfo = _bindingMap[descriptorName];
    _instances[handle].SetWrites[descriptorInfo.BindingIndex].pNext = &ASdescriptorWrite;
}


void DescriptorSet::UpdateSetInstance(SetInstanceHandle handle)
{
    vkUpdateDescriptorSets(_instance->GetDevice(), _descriptorsCount, _instances[handle].SetWrites, 0, nullptr);
}

void DescriptorSet::CreateBufferView(Instance* instance, BufferInformation &bufferInfo, VkFormat format)
{
    //If a buffer already has a view specified
    if(bufferInfo.BufferView != VK_NULL_HANDLE) return;
    
    VkBufferViewCreateInfo bufferViewInfo;
    bufferViewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    bufferViewInfo.pNext = nullptr;
    bufferViewInfo.flags = 0;
    bufferViewInfo.buffer = bufferInfo.buffer;
    bufferViewInfo.format = format;
    bufferViewInfo.offset = 0;
    bufferViewInfo.range = bufferInfo.size;
    
    VkResult code = vkCreateBufferView(instance->GetDevice(), &bufferViewInfo, nullptr, &bufferInfo.BufferView);
    Utilities::CheckVkResult(code, "Could not create a buffer view!");
}

};