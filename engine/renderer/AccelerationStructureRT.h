#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <vector>

#include "DeviceMemoryManager.h"
#include "RenderLogicStructs.h"
#include "RTUtilities.h"
namespace RTE::Rendering
{

/**
 * @brief Acceleration structure used for ray tracing. 
 *        Encapsulates the creation of Vulkan bot and top level structures along with rebuilding the top structures when instance transforms change.
 */
class AccelerationStructure
{
    /**
     * @brief Pointer to the instance.
     */
    Instance *_instance = nullptr;

    /**
     * @brief Pointer to the device memory manager.
     */
    DeviceMemoryManager *_memoryManager = nullptr;
    
    /**
     * @brief Pointer to the command buffer manager.
     */
    CommandBufferManager *_commandBufferManager = nullptr;

    /**
     * @brief Pointer to the ray tracing utilities.
     */
    RTUtilities *_rtUtil = nullptr;

    /**
     * @brief Vulkan handle to the top acceleration structure.
     */
    VkAccelerationStructureNV _top = VK_NULL_HANDLE;

    /**
     * @brief Vector of Vulkan handles to bottom level acceleration structures.
     */
    std::vector<VkAccelerationStructureNV> _botStructures;

    /**
     * @brief Vector of Vulkan handles to bottom level acceleration structures used for geometry instances.
     */
    std::vector<uint64_t> _botStructureHandles;

    /**
     * @brief Vector mapping from the index corresponing to the instance handle, to the mesh handle.
     */
    std::vector<MeshHandle> _instanceMeshMapping;

    /**
     * @brief Vector of Vulkan geometries used to create and build bottom level acceleration structures
     */
    std::vector<VkGeometryNV> _geometries;

    /**
     * @brief Vector of geometry instances used to build the top level structure.
     */
    std::vector<VkGeometryInstance> _instances;

    /**
     * @brief Allocation information on the scratch buffer.
     */
    Buffer _scratchBuffer; 

    /**
     * @brief Allocation information on the instance buffer.
     */
    Buffer _instanceBuffer; 

    /**
     * @brief Descriptor write for the acceleration structure. WARNING: Allows descriptor count of 1 only (cannot have arrays of acceleration structures in the shader)
     * 
     */
    VkWriteDescriptorSetAccelerationStructureNV _descriptorWrite;

    /**
     * @brief Creates an acceleration structure.
     * @param type The type of the acceleration structure.
     * @param geometryCount The number of geometries, if creating a bottom level structure.
     * @param geometries Pointer to the geometries.
     * @param instanceCount The number of instances, if creating a top level structure.
     * @param AS The structure handle to be set.
     */
    void CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV* geometries, uint32_t instanceCount, VkAccelerationStructureNV& AS);
    
    /**
     * @brief Fills the bottom level structures with the geometry data and the top level structure with the intances data.
     */
    void FillStructures();

    /**
     * @brief Converts a MeshInfo object to VkGeometry object required for the bottom level structure.
     * @param mesh The MeshInfo object.
     * @return Returns a geometry object.
     */
    VkGeometryNV CreateGeometry(MeshInfo &mesh);

    /**
     * @brief Creates the bottom level acceleration structures, one for each mesh.
     * @param meshes All the mesh data required for rendering.
     */
    void CreateBotStructures(std::vector<MeshInfo> &meshes);

    /**
     * @brief Converts a MeshInstance object to VkGeometryInstance object required for the top level structure.
     * @param instance The MeshInstance object.
     * @param instanceId The id of the instance, corresponding to the instance handle.
     * @return Returns a geometry instance object.
     */
    VkGeometryInstance ConvertInstance(MeshInstance &instance, uint32_t instanceId);

    /**
     * @brief Creates the top level acceleration structure.
     * @param instances The instances of geometries.
     */
    void CreateTopStructure(std::vector<MeshInstance> &instances);

public:
    AccelerationStructure(  Instance *instance, DeviceMemoryManager *deviceMemoryManager, 
                            CommandBufferManager *commandBufferManager, std::vector<MeshInfo> &meshes,
                            std::vector<MeshInstance> &instances);

    /**
     * @brief Gets the top level acceleration structure handle.
     * @return Returns a Vulkan acceleration structure hanle.
     */
    VkAccelerationStructureNV& GetTopStructure();

    /**
     * @brief Gets the bottom level acceleration structure handles.
     * @return Returns a vector of Vulkan acceleration structure hanles.
     */
    std::vector<VkAccelerationStructureNV> GetBotStructures();

    
    /**
     * @brief Get the Descriptor Write for the descriptor set.
     * 
     * @return Vulkan VkWriteDescriptorSetAccelerationStructureNV struct.
     */
    VkWriteDescriptorSetAccelerationStructureNV& GetDescriptorWrite();


    /**
     * @brief Updates an instance transform.
     * @param instanceHandle The handle to the instance.
     * @param modelMatrix The transformation matrix to be set for the instance.
     */
    void UpdateInstanceTransform(MeshInstanceHandle instanceHandle, glm::mat4 modelMatrix);

    /**
     * @brief Rebuilds the top level acceleration structure. Needed after modifying instance transforms.
     * @param commandBuffer command buffer used to execute the update command.
     */
    void RebuildTopStructureCmd(VkCommandBuffer commandBuffer);
};

}; // RTE::Rendering
