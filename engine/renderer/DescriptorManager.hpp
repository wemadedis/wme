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

    std::vector<VkDescriptorSetLayout> _layoutsRT;
    VkDescriptorPool _poolRT;
    std::vector<VkDescriptorSet> _descriptorsetsRT;

public:
    DescriptorManager(Instance *instance);
    void CreateDescriptorPool(SwapChain *swapChain, std::vector<MeshInstance> &meshes);
    void CreateDescriptorSetLayout();
    void CreateDescriptorSetLayoutRT(uint32_t meshCount, uint32_t instanceCount);
    void CreateDescriptorSetRT(AccelerationStructure *AS, VkImageView imageViewRT, BufferInformation &globalUniform, std::vector<MeshInfo*> meshes, std::vector<MeshInstance> instances, BufferInformation &instanceBuffer);
    void CreateDescriptorSets(std::vector<MeshInstance> &instances, std::vector<TextureInfo> textures, BufferInformation &globalUniformData);
    VkDescriptorSetLayout* GetDescriptorLayout();
    std::vector<VkDescriptorSetLayout> GetDescriptorLayoutRT();
    VkDescriptorPool GetDescriptorPool();
    std::vector<VkDescriptorSet> GetDescriptorSets();
    std::vector<VkDescriptorSet> GetDescriptorSetRT();
};
};