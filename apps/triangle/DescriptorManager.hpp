#pragma once
#include <vector>

#include <vulkan/vulkan.h>


#include "Instance.hpp"
#include "SwapChain.hpp"
#include "Mesh.hpp"

class DescriptorManager
{
    Instance *_instance;
    VkDescriptorSetLayout _layout;
    VkDescriptorPool _pool;
    std::vector<VkDescriptorSet> _descriptorSets;
public:
    DescriptorManager(Instance *instance);
    VkDescriptorPool CreateDescriptorPool(SwapChain *swapChain);
    VkDescriptorSetLayout CreateDescriptorSetLayout();
    std::vector<VkDescriptorSet> CreateDescriptorSets(std::vector<Mesh*> &meshes);
};