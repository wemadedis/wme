#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <vector>

#include "DeviceMemoryManager.h"
#include "RenderLogicStructs.h"
#include "RTUtilities.h"
namespace RTE::Rendering
{


class AccelerationStructure
{
    Instance *_instance = nullptr;
    DeviceMemoryManager *_memoryManager = nullptr;
    CommandBufferManager *_commandBufferManager = nullptr;
    RTUtilities *_rtUtil = nullptr;
    VkAccelerationStructureNV _top = VK_NULL_HANDLE;
    std::vector<VkAccelerationStructureNV> _bot;
    std::vector<MeshHandle> _instanceMeshMapping;
    uint32_t _instanceCount = 0;

    BufferInformation _scratchBuffer; 
    BufferInformation _instanceBuffer; 
    
    BufferInformation _vertexBuffer;
    BufferInformation _indexBuffer;

    void CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV* geometries, uint32_t instanceCount, VkAccelerationStructureNV& AS);
    void CreateBotTopStructures(std::vector<VkGeometryNV> geometries);
    void FillOutStructure(std::vector<VkGeometryNV> geometries, std::vector<VkGeometryInstance> meshInstances);

public:
    AccelerationStructure(  Instance *instance, DeviceMemoryManager *deviceMemoryManager, 
                            CommandBufferManager *commandBufferManager, std::vector<MeshInfo*> &meshes,
                            std::vector<MeshInstance> &instances);
    VkAccelerationStructureNV& GetTopStructure();
    std::vector<VkAccelerationStructureNV> GetBotStructures();
    void UpdateInstanceTransform(MeshInstanceHandle instanceHandle, glm::mat4 modelMatrix);
    void RebuildTopStructureCmd(VkCommandBuffer commandBuffer);
};


}; // RTE::Rendering
