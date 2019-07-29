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
    };

    struct DescriptorInfo
    {
        VkDescriptorSetLayoutBinding Binding;
        VkDescriptorSetLayout Layout;
        uint32_t LayoutIndex;
        uint32_t DescriptorIndex;
    };

    struct SetInstance
    {
        std::vector<VkDescriptorSet> Sets;
        std::vector<VkWriteDescriptorSet> SetWrites;
    };
    
    Instance* _instance;
    uint32_t _descriptorsCount = 0;
    std::vector<VkDescriptorSetLayout> _setLayouts;
    std::vector<LayoutInfo>* _layoutInfos;
    VkPipelineLayout _pipelineLayout;
    

    std::unordered_map<std::string, DescriptorInfo> _bindingMap;
    std::vector<VkDescriptorPoolSize> _poolSizes = {};
    std::vector<VkDescriptorPool> _descriptorPools = {};
    
    SetInstanceHandle _setAllocationsCount = 0;
    std::unordered_map<SetInstanceHandle, SetInstance> _instances = {};
    uint32_t _maxSetsPerPool = 0;
    
    AllocationInfo _allocationInfo;
    uint32_t _poolAllocationCount = 0;
    uint32_t _currentPoolIndex = 0;
    
    DescriptorSet(std::vector<LayoutInfo>* layoutInfos, Instance* instance, uint32_t maxSets);
    

    void CreatePipelineLayout();
    void CreateDescriptorPool();
    void CreateSetAllocationInfo();
    VkWriteDescriptorSet GetDescriptorWrite(std::string descriptorName, VkDescriptorSet set);

public:
    class DescriptorSetBuilder
    {
    private:
        std::map<std::string,VkDescriptorSetLayoutBinding> _layoutBindings = {};
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
    
    /**
	 * @brief Updates a uniform buffer. Includes texel buffers with a buffer view specified.
	 */
    void UpdateUniformBuffer(SetInstanceHandle handle, std::string descriptorName, BufferInformation *bufferInfos, uint32_t bufferCount);
    void UpdateImage(SetInstanceHandle handle, std::string descriptorName, ImageInfo *imageInfos, uint32_t imageCount);
    void UpdateAccelerationStructure(SetInstanceHandle handle, std::string descriptorName, AccelerationStructure* as);

    /**
	 * @brief Updates the whole set based on the previous descriptor updates. Should be the last update call.
	 */
    void UpdateSetInstance(SetInstanceHandle handle);

    static void CreateBufferView(Instance* instance, BufferInformation &bufferInfo, VkFormat format);

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
};

};