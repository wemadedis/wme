#pragma once
#include <vector>

#include <vulkan/vulkan.h>

#include "Instance.hpp"
#include "SwapChain.hpp"
#include "RenderLogicStructs.h"
#include "AccelerationStructureRT.h"

namespace RTE::Rendering
{
/**
 * @brief Creates and updates descriptors for the data used in various shaders.
 */
class DescriptorManager
{
    /**
     * @brief Pointer to the instance.
     */
    Instance *_instance;

    /**
     * @brief Vulkan handle to the descriptor set layout used in rasterization.
     */
    VkDescriptorSetLayout _layout;
    
    /**
     * @brief 
     */
    VkDescriptorPool _pool;
    
    /**
     * @brief
     */
    std::vector<VkDescriptorPool> _pools;
    
    /**
     * @brief
     */
    std::vector<VkDescriptorSet> _descriptorSets;

    /**
     * @brief
     */
    std::vector<VkDescriptorSetLayout> _layoutsRT;
    
    /**
     * @brief
     */
    VkDescriptorPool _poolRT;

    /**
     * @brief
     */
    std::vector<VkDescriptorSet> _descriptorsetsRT;

    /**
     * @brief
     */
    VkWriteDescriptorSetAccelerationStructureNV _descriptorAccelerationStructureInfo = {};

    /**
     * @brief
     */
    VkDescriptorImageInfo _descriptorOutputImageInfo = {};

    /**
     * @brief
     */
    VkDescriptorBufferInfo _globalBuffer = {};

    /**
     * @brief
     */
    VkBufferView _mappingView;

    /**
     * @brief
     */
    std::vector<VkBufferView> _indexViews;

    /**
     * @brief
     */
    std::vector<VkBufferView> _vertexViews;

    /**
     * @brief
     */
    std::vector<VkDescriptorBufferInfo> _meshBufferInfos;

    /**
     * @brief
     */
    std::vector<VkDescriptorImageInfo> _imageInfos;

    /**
     * @brief
     */
    std::vector<VkWriteDescriptorSet> _descriptorWritesRT;
public:

    /**
     * @brief
     */
    DescriptorManager(Instance *instance);

    /**
     * @brief
     */
    void CreateDescriptorPool(SwapChain *swapChain, std::vector<MeshInstance> &meshes);

    /**
     * @brief
     */
    void CreateDescriptorSetLayout();

    /**
     * @brief
     */
    void CreateDescriptorSetLayoutRT(uint32_t meshCount, uint32_t instanceCount, uint32_t textureCount);

    /**
     * @brief
     */
    void CreateDescriptorSetRT(AccelerationStructure *AS, VkImageView imageViewRT, BufferInformation &globalUniform, std::vector<MeshInfo> meshes, std::vector<MeshInstance> instances, BufferInformation &instanceBuffer, std::vector<TextureInfo> textures);
    
    /**
     * @brief
     */
    void CreateDescriptorSets(std::vector<MeshInstance> &instances, std::vector<TextureInfo> textures, BufferInformation &globalUniformData);

    /**
     * @brief
     */
    void UpdateRTTargetImage(VkImageView imageViewRT);
    
    /**
     * @brief
     */
    VkDescriptorSetLayout* GetDescriptorLayout();

    /**
     * @brief
     */
    std::vector<VkDescriptorSetLayout> GetDescriptorLayoutRT();

    /**
     * @brief
     */
    VkDescriptorPool GetDescriptorPool();

    /**
     * @brief
     */
    std::vector<VkDescriptorSet> GetDescriptorSets();

    /**
     * @brief
     */
    std::vector<VkDescriptorSet> GetDescriptorSetRT();
};
};