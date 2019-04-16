#pragma once

#include "vulkan/vulkan.h"

namespace RTE::Rendering
{

#define NVVK_RESOLVE_INSTANCE_FUNCTION_ADDRESS(instance, funcName) \
{ \
    funcName = reinterpret_cast<PFN_##funcName>(vkGetInstanceProcAddr(instance, ""#funcName)); \
    if (funcName == nullptr) \
    { \
        const std::string name = #funcName; \
        /*ExitError(std::wstring(L"Can't get instance function address: ") + std::wstring(name.begin(), name.end()));*/ \
    } \
}

#define NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, funcName) \
{ \
    funcName = reinterpret_cast<PFN_##funcName>(vkGetDeviceProcAddr(device, ""#funcName)); \
    if (funcName == nullptr) \
    { \
        const std::string name = #funcName; \
        /*ExitError(std::wstring(L"Can't get device function address: ") + std::wstring(name.begin(), name.end()));*/ \
    } \
}
class RTUtilities
{
    RTUtilities(VkDevice device)
    {
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkCreateAccelerationStructureNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkDestroyAccelerationStructureNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkGetAccelerationStructureMemoryRequirementsNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkCmdCopyAccelerationStructureNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkBindAccelerationStructureMemoryNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkCmdBuildAccelerationStructureNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkCmdTraceRaysNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkGetRayTracingShaderGroupHandlesNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkCreateRayTracingPipelinesNV);
        NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, vkGetAccelerationStructureHandleNV);
    }
    
public:
    static RTUtilities* GetInstance(VkDevice *device = nullptr)
    {
        static RTUtilities *_instance = nullptr;
        if(device == nullptr && _instance == nullptr)
        {
            //TODO: Throw some proper exception
            throw "U done goofd";
        }
        if(_instance == nullptr)
        {
            _instance = new RTUtilities(*device);
        }
        return _instance;
    }

    PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
    PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
    PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
    PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
    PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
    PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
    PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
    PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
    PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
    PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;

};

};