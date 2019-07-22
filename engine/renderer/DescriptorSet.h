#pragma once
#include <vulkan/vulkan.h>
#include <map>
#include <string>
#include <vector>

namespace RTE::Rendering
{

class DescriptorSet
{
private:
    struct DescriptorSetLayoutInfo
    {
        VkDescriptorSetLayout Layout;
        std::map<std::string, VkDescriptorSetLayoutBinding> Bindings;
    };
    DescriptorSet(std::vector<DescriptorSetLayoutInfo> setLayouts);

    std::vector<DescriptorSetLayoutInfo> _setLayouts;


public:
    class DescriptorSetBuilder
    {
    private:
        std::map<std::string,VkDescriptorSetLayoutBinding> _layoutBindings = {};
        std::map<std::string,VkWriteDescriptorSet> _setWrites = {};
        uint32_t _currentBinding = 0;

        DescriptorSetBuilder& AddLayoutBinding(std::string name, VkDescriptorType descriptorType, uint32_t descriptorCount, VkShaderStageFlags shaderStages);
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
        DescriptorSet* Build();
    };

    uint32_t Allocate();
    void UpdateUnifromBuffer();
    void UpdateUniformTexelBuffer();
    void UpdateCombinedImageSampler();
    void UpdateStorageImage();
    void UpdateAccelerationStructure();
};

};