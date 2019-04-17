#pragma once

#include <string>

#include "vulkan/vulkan.h"
#include "RTEException.h"

#define GET_FUNCTION_NAME(function) ""#function

namespace RTE::Rendering
{

class RTUtilities
{

    #define NVVK_RESOLVE_DEVICE_FUNCTION_ADDRESS(device, funcName) \
    { \
        funcName = reinterpret_cast<PFN_##funcName>(vkGetDeviceProcAddr(device, ""#funcName)); \
        if (funcName == nullptr) \
        { \
            throw RTEException((std::string("Could not resolve function address: ")+std::string(""#funcName)).c_str()); \
        } \
    }
/*
    PFN_vkVoidFunction ResolveFuntionAddress(VkDevice device, const char *functionName)
    {
        auto func = vkGetDeviceProcAddr(device, functionName);
        if(func == nullptr)
        {
            throw RTEException((std::string("Could not resolve function address: ")+std::string(functionName)).c_str());
        }
        return func;
    }
*/
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