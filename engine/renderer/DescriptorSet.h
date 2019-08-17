#pragma once
#include <vulkan/vulkan.h>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include "Instance.hpp"
#include "DeviceMemoryManager.h"
#include "RenderLogicStructs.h"
#include "AccelerationStructureRT.h"

namespace RTE::Rendering
{

typedef uint32_t SetInstanceHandle;

class DescriptorSet
{
private:
    struct DescriptorInfo
    {
        VkDescriptorSetLayoutBinding Binding;
        VkFormat Format = VK_FORMAT_UNDEFINED;
        VkDescriptorSetLayout Layout;
        uint32_t LayoutIndex;
        uint32_t DescriptorIndex;
    };

    struct LayoutInfo
    {
        VkDescriptorSetLayout Layout = {};
        bool HasVariableSizeBinding = false;
        std::vector<std::string> Descriptors = {};
    };

    struct AllocationInfo
    {
        std::vector<uint32_t> VariableDescriptorCounts;
        VkDescriptorSetVariableDescriptorCountAllocateInfoEXT VariableDescriptorCountInfo;
        VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo;
    };

    struct SetInstance
    {
        std::vector<VkDescriptorSet> Sets;
        std::vector<VkWriteDescriptorSet> SetWrites;
    };
    
    Instance* _instance;
    uint32_t _descriptorsCount = 0;
    std::vector<VkDescriptorSetLayout> _setLayouts;
    std::vector<LayoutInfo> _layoutInfos;
    VkPipelineLayout _pipelineLayout;
    
    //Assuming only one buffer view per buffer per instance of this class
    std::unordered_map<VkBuffer, VkBufferView> _bufferViews;
    std::unordered_map<std::string, DescriptorInfo> _bindingMap;
    std::vector<VkDescriptorPoolSize> _poolSizes = {};
    std::vector<VkDescriptorPool> _descriptorPools = {};
    
    SetInstanceHandle _setAllocationsCount = 0;
    std::unordered_map<SetInstanceHandle, SetInstance> _instances = {};
    uint32_t _maxSetsPerPool = 0;
    
    AllocationInfo _allocationInfo;
    uint32_t _poolAllocationCount = 0;
    uint32_t _currentPoolIndex = 0;

    

    void CreatePipelineLayout();
    void CreateDescriptorPool();
    void CreateSetAllocationInfo();
    VkWriteDescriptorSet GetDescriptorWrite(std::string descriptorName, VkDescriptorSet set);
    void CreateBufferView(SetInstanceHandle handle, std::string descriptorName , Buffer &bufferInfo);
public:
    class DescriptorSetBuilder
    {
    private:
        std::map<std::string,VkDescriptorSetLayoutBinding> _layoutBindings = {};
        std::map<std::string, VkFormat> _descriptorFormats = {};
        std::vector<LayoutInfo> _layoutInfos = {{}};
        uint32_t _currentBinding = 0;
        void CreateDescriptorSetLayout(LayoutInfo& layoutInfo, Instance* instance);

        DescriptorSetBuilder() = default;

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
                                                     VkFormat viewFormat,
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

        friend class DescriptorSet;
    };

    static DescriptorSetBuilder Create();

    VkPipelineLayout GetPipelineLayout();
    SetInstanceHandle Allocate();
    std::vector<VkDescriptorSet> GetVkDescriptorSets();
    //TODO: Only used by ShaderGenerator, try to declare it a friend class instead!
    std::unordered_map<std::string, DescriptorInfo> GetBindingMap();
    /**
	 * @brief Updates a uniform buffer. Includes texel buffers with a buffer view specified.
	 */
    void UpdateUniformBuffer(SetInstanceHandle handle, std::string descriptorName, Buffer *bufferInfos, uint32_t bufferCount, VkDescriptorBufferInfo* descriptorBufferInfo = nullptr);
    void UpdateUniformTexelBuffer(SetInstanceHandle handle, std::string descriptorName, Buffer *bufferInfos, uint32_t bufferCount);
    void UpdateImage(SetInstanceHandle handle, std::string descriptorName, Image *imageInfos, uint32_t imageCount);
    void UpdateAccelerationStructure(SetInstanceHandle handle, std::string descriptorName, AccelerationStructure* as);

    /**
	 * @brief Updates the whole set based on the previous descriptor updates. Should be the last update call.
	 */
    void UpdateSetInstance(SetInstanceHandle handle);

    //TODO: Generate initial shader code?
    std::string GetBindingsInformation()
    {
        std::string info = "";
        for(auto binding : _bindingMap)
        {
            info = info + std::string("layout(set = ") 
                        + std::to_string(binding.second.LayoutIndex) + std::string(", binding = ") 
                        + std::to_string(binding.second.Binding.binding)
                        + std::string(") ") + binding.first + std::string("\n");
        }
        return info;
    }

    private:
        DescriptorSet(DescriptorSetBuilder *builder, Instance* instance, uint32_t maxSets);
};

};