#pragma once
#include <vector>

#include <vulkan/vulkan.h>

#include "Instance.hpp"
#include "SwapChain.hpp"
#include "RenderLogicStructs.h"
#include "AccelerationStructureRT.h"
#include "DescriptorSet.h"

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
     * @brief Vulkan handle to the descriptor pool used by a GUI module.
     */
    VkDescriptorPool _pool;
    
    /**
     * @brief Vulkan handles to the descriptor pool used for meshes (rasterization).
     */
    std::vector<VkDescriptorPool> _pools;
    
    /**
     * @brief Vulkan handles to the descriptor sets used for meshes (rasterization).
     */
    std::vector<VkDescriptorSet> _descriptorSets;

    /**
     * @brief Vulkan handles to descriptor set layouts (ray tracing).
     */
    std::vector<VkDescriptorSetLayout> _layoutsRT;
    
    /**
     * @brief Vulkan handle to the descriptor pool (ray tracing).
     */
    VkDescriptorPool _poolRT;

    /**
     * @brief Vulkan handles to descriptor sets (ray tracing).
     */
    std::vector<VkDescriptorSet> _descriptorsetsRT;

    /**
     * @brief Acceleration structure descriptor set write info.
     */
    VkWriteDescriptorSetAccelerationStructureNV _descriptorAccelerationStructureInfo = {};

    /**
     * @brief Image descriptor info.
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

    DescriptorSet* _rasterizationDescriptorSet;
    DescriptorSet* _raytracingDescriptorSet;
    SetInstanceHandle rtSetHandle;
public:

    /**
     * @brief The Descriptor Manager constructor.
     * @param instance Pointer to the instance.
     */
    DescriptorManager(Instance *instance);

    /**
     * @brief Creates the descriptor pools used for rasterization.
     * @param swapChain Pointer to the swap chain.
     * @param instances Mesh instances used for rendering.
     */
    void CreateDescriptorPools(SwapChain *swapChain, std::vector<MeshInstance> &instances);

    /**
     * @brief Creates the descriptor set layout (rasterization).
     */
    void CreateDescriptorSetLayout();

    /**
     * @brief Creates the descriptor set layout (ray tracing).
     * @param meshCount The total number of meshes.
     * @instanceCount The total number of instances.
     * @textureCount The total number of textures.
     */
    void CreateDescriptorSetLayoutRT(uint32_t meshCount, uint32_t instanceCount, uint32_t textureCount);

    /**
     * @brief Creates the descriptor set (ray tracing).
     */
    void CreateDescriptorSetRT(AccelerationStructure *AS, ImageInfo imageViewRT, BufferInformation &globalUniform, std::vector<MeshInfo> meshes, std::vector<MeshInstance> instances, BufferInformation &instanceBuffer, std::vector<TextureInfo> textures);
    
    /**
     * @brief Creates the descriptor sets (rasterization)
     * @param instances Vector of instances.
     * @param textures Vector of texture infos.
     * @param globalUniformData Global uniform buffer memory allocation information.
     */
    void CreateDescriptorSets(std::vector<MeshInstance> &instances, std::vector<TextureInfo> textures, BufferInformation &globalUniformData);

    /**
     * @brief Updated the current framebuffer image view for ray tracing.
     */
    void UpdateRTTargetImage(ImageInfo imageViewRT);
    
    /**
     * @brief Gets the descriptor set layout (rasterization)
     */
    VkPipelineLayout GetDescriptorLayout();

    /**
     * @brief Gets the descriptor set layouts (ray tracing)
     */
    VkPipelineLayout GetDescriptorLayoutRT();

    /**
     * @brief Gets the descriptor pool (for GUI).
     */
    VkDescriptorPool GetDescriptorPool();

    /**
     * @brief Gets the descriptor sets (rasterization)
     */
    std::vector<VkDescriptorSet> GetDescriptorSets();

    /**
     * @brief Gets the descriptor sets (ray tracing)
     */
    std::vector<VkDescriptorSet> GetDescriptorSetRT();
};
};