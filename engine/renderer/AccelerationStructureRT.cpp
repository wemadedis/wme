#include "AccelerationStructureRT.h"
#include "RTUtilities.h"
namespace RTE::Rendering
{

void AccelerationStructure::CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV* geometries, uint32_t instanceCount, VkAccelerationStructureNV& AS, VkDeviceMemory& memory)
{
    VkAccelerationStructureCreateInfoNV accelerationStructureInfo;
    accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
    accelerationStructureInfo.pNext = nullptr;
    accelerationStructureInfo.compactedSize = 0;
    accelerationStructureInfo.info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
    accelerationStructureInfo.info.pNext = NULL;
    accelerationStructureInfo.info.type = type;
    accelerationStructureInfo.info.flags = 0;
    accelerationStructureInfo.info.instanceCount = instanceCount;
    accelerationStructureInfo.info.geometryCount = geometryCount;
    accelerationStructureInfo.info.pGeometries = geometries;

    VkResult code = RTUtilities::GetInstance()->vkCreateAccelerationStructureNV(_instance->GetDevice(), &accelerationStructureInfo, nullptr, &AS);
    //NVVK_CHECK_ERROR(code, L"vkCreateAccelerationStructureNV");

    
    //NVVK_CHECK_ERROR(code, L"vkBindAccelerationStructureMemoryNV");
}

void AccelerationStructure::CreateBottomStructure(std::vector<VkGeometryNV> geometries)
{
    
}


AccelerationStructure::AccelerationStructure(Instance *instance, DeviceMemoryManager *deviceMemoryManager)
{
    _instance = instance;
    _memoryManager = deviceMemoryManager; 
}

}