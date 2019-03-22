#pragma once
#include <vector>

#include <vulkan/vulkan.h>

#include "Instance.hpp"
#include "SwapChain.hpp"
#include "RenderLogicStructs.h"
namespace RTE::Renderer
{

class DescriptorManager
{
    Instance *_instance;
    VkDescriptorSetLayout _layout;
    VkDescriptorPool _pool;
    std::vector<VkDescriptorSet> _descriptorSets;
public:
    DescriptorManager(Instance *instance);
    void CreateDescriptorPool(SwapChain *swapChain);
    void CreateDescriptorSetLayout();
    void CreateDescriptorSets(std::vector<MeshInfo*> &meshes, BufferInformation &globalUniformData);
    VkDescriptorSetLayout GetDescriptorLayout();
    VkDescriptorPool GetDescriptorPool();
    std::vector<VkDescriptorSet> GetDescriptorSets();
};
};