#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <vector>

#include "DeviceMemoryManager.h"

namespace RTE::Rendering
{


class AccelerationStructure
{
    Instance *_instance = nullptr;
    DeviceMemoryManager *_memoryManager = nullptr;
    
    
    VkAccelerationStructureNV _top = VK_NULL_HANDLE;
    VkAccelerationStructureNV _bot = VK_NULL_HANDLE;


    void CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV* geometries, uint32_t instanceCount, VkAccelerationStructureNV& AS, VkDeviceMemory& memory);
    void CreateBottomStructure(std::vector<VkGeometryNV> geometries);
    void CreateTopStructure();

public:
    AccelerationStructure(Instance *instance, DeviceMemoryManager *deviceMemoryManager);

};


}; // RTE::Rendering
