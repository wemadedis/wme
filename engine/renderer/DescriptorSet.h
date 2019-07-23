#pragma once
#include <vulkan/vulkan.h>
#include <map>
#include <string>
#include <vector>


#include "Instance.hpp"

namespace RTE::Rendering
{

class DescriptorSet
{
private:
    struct LayoutInfo
    {
        VkDescriptorSetLayout Layout = {};
        bool HasVariableSizeBinding = false;
        std::map<std::string, VkDescriptorSetLayoutBinding> Bindings = {};
    };
    DescriptorSet(std::vector<LayoutInfo>* setLayouts);

    std::vector<LayoutInfo> _setLayouts;


public:
    class DescriptorSetBuilder
    {
    private:
        std::map<std::string,VkDescriptorSetLayoutBinding> _layoutBindings = {};
        uint32_t _currentBinding = 0;

        void CreateDescriptorSetLayout(LayoutInfo& layoutInfo);

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
        DescriptorSet* Build(Instance* instance);
    };

    uint32_t Allocate();
    void UpdateUnifromBuffer();
    void UpdateUniformTexelBuffer();
    void UpdateCombinedImageSampler();
    void UpdateStorageImage();
    void UpdateAccelerationStructure();
};

};