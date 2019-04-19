#pragma once
#include <vector>

#include <vulkan/vulkan.h>

#include "Instance.hpp"
#include "SwapChain.hpp"
#include "RenderLogicStructs.h"
#include "AccelerationStructureRT.h"

namespace RTE::Rendering
{

class DescriptorManager
{
    Instance *_instance;
    VkDescriptorSetLayout _layout;
    VkDescriptorPool _pool;
    std::vector<VkDescriptorPool> _pools;
    std::vector<VkDescriptorSet> _descriptorSets;

    VkDescriptorSetLayout _layoutRT;
    VkDescriptorPool _poolRT;
    VkDescriptorSet _descriptorsetRT;

public:
    DescriptorManager(Instance *instance);
    void CreateDescriptorPool(SwapChain *swapChain, std::vector<MeshInstance> &meshes);
    void CreateDescriptorSetLayout();
    void CreateDescriptorSetLayoutRT();
    void CreateDescriptorSetRT(AccelerationStructure *AS, VkImageView imageViewRT);
    void CreateDescriptorSets(std::vector<MeshInstance> &instances, std::vector<TextureInfo> textures, BufferInformation &globalUniformData);
    VkDescriptorSetLayout* GetDescriptorLayout();
    VkDescriptorSetLayout* GetDescriptorLayoutRT();
    VkDescriptorPool GetDescriptorPool();
    std::vector<VkDescriptorSet> GetDescriptorSets();
    VkDescriptorSet GetDescriptorSetRT();
};
};