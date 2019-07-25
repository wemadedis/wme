#pragma once
#include <vulkan/vulkan.h>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include "Instance.hpp"

namespace RTE::Rendering
{

typedef uint32_t SetInstanceHandle;

class DescriptorSet
{
private:
    struct LayoutInfo
    {
        VkDescriptorSetLayout Layout = {};
        bool HasVariableSizeBinding = false;
        std::map<std::string, VkDescriptorSetLayoutBinding> Bindings = {};
    };

    struct AllocationInfo
    {
        std::vector<uint32_t> VariableDescriptorCounts;
        VkDescriptorSetVariableDescriptorCountAllocateInfoEXT VariableDescriptorCountInfo;
        VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo;
    } _allocationInfo;
    
    Instance* _instance;
    std::vector<VkDescriptorSetLayout> _setLayouts;
    std::vector<LayoutInfo>* _layoutInfos;
    VkPipelineLayout _pipelineLayout;
    

    std::unordered_map<std::string, std::pair<VkDescriptorSetLayoutBinding, VkDescriptorSetLayout>> _bindingMap;
    std::vector<VkDescriptorPoolSize> _poolSizes = {};
    std::vector<VkDescriptorPool> _descriptorPools = {};
    std::vector<VkDescriptorSet> _descriptorSets = {};
    uint32_t _maxSetsPerPool = 0;
    uint32_t _poolAllocationCount = 0;
    uint32_t _currentPoolIndex = 0;
    DescriptorSet(std::vector<LayoutInfo>* layoutInfos, Instance* instance, uint32_t maxSets);
    

    void CreatePipelineLayout();
    void CreateDescriptorPool();
    void CreateSetAllocationInfo();
    void UpdateDescriptor(SetInstanceHandle handle);
public:
    class DescriptorSetBuilder
    {
    private:
        std::map<std::string,VkDescriptorSetLayoutBinding> _layoutBindings = {};
        uint32_t _currentBinding = 0;
        void CreateDescriptorSetLayout(LayoutInfo& layoutInfo, Instance* instance);

        DescriptorSetBuilder& AddLayoutBinding(std::string name, 
                                               VkDescriptorType descriptorType, 
                                               uint32_t descriptorCount, 
                                               VkShaderStageFlags shaderStages);
    public:
        DescriptorSetBuilder& WithUniformBuffer(std::string name,
                                                uint32_t descriptorCount,
                                                VkShaderStageFlags shaderStages);

        DescriptorSetBuilder& WithUniformTexelBuffer(std::string name,
                                                     uint32_t descriptorCount,
                                                     VkShaderStageFlags shaderStages);                                                  
        
        DescriptorSetBuilder& WithCombinedImageSampler(std::string name,
                                                       uint32_t descriptorCount,
                                                       VkShaderStageFlags shaderStages);

        DescriptorSetBuilder& WithStorageImage(std::string name,
                                               uint32_t descriptorCount,
                                               VkShaderStageFlags shaderStages);           

        DescriptorSetBuilder& WithAccelerationStructure(std::string name,
                                                        uint32_t descriptorCount,
                                                        VkShaderStageFlags shaderStages);

        DescriptorSet* Build(Instance* instance, uint32_t maxSetsPerPool = 100);
    };

    VkPipelineLayout GetPipelineLayout();
    SetInstanceHandle Allocate();

    void UpdateUnifromBuffer(SetInstanceHandle handle);
    void UpdateUniformTexelBuffer();
    void UpdateCombinedImageSampler();
    void UpdateStorageImage();
    void UpdateAccelerationStructure();
};

};