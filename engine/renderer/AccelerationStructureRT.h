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
    VkAccelerationStructureNV _bot = VK_NULL_HANDLE;

    //Is this one necessary to keep track of?
    BufferInformation _scratchBuffer; 
    BufferInformation _instanceBuffer; 
    
    BufferInformation _vertexBuffer;
    BufferInformation _indexBuffer;

    void CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV* geometries, uint32_t instanceCount, VkAccelerationStructureNV& AS);
    void CreateBotTopStructures(std::vector<VkGeometryNV> geometries);
    void FillOutStructure(std::vector<VkGeometryNV> geometries, std::vector<VkGeometryInstance> instances);

public:
    AccelerationStructure(Instance *instance, DeviceMemoryManager *deviceMemoryManager, CommandBufferManager *commandBufferManager);
    VkAccelerationStructureNV GetTopStructure();
    VkAccelerationStructureNV GetBotStructure();
};


}; // RTE::Rendering
