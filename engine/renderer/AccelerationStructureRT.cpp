#include "AccelerationStructureRT.h"
#include "RTUtilities.h"
namespace RTE::Rendering
{

void AccelerationStructure::CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV* geometries, uint32_t instanceCount, VkAccelerationStructureNV &AS)
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

    VkResult code = _rtUtil->vkCreateAccelerationStructureNV(_instance->GetDevice(), &accelerationStructureInfo, nullptr, &AS);
    if(code != VK_SUCCESS)
    {
        auto errorMessage = (std::string("Could not create acceleration structure! Error: ")+std::to_string(code)).c_str();
        throw RTEException(errorMessage);
    }
    _memoryManager->AllocateAccelerationStructureMemory(AS);
    
    //NVVK_CHECK_ERROR(code, L"vkBindAccelerationStructureMemoryNV");
}

void AccelerationStructure::CreateBotTopStructures(std::vector<VkGeometryNV> geometries)
{
    CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV, (uint32_t)geometries.size(), geometries.data(), 0, _bot);
    int i = 0;
    CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV, 0, nullptr, 1, _top);
}

void AccelerationStructure::FillOutStructure(std::vector<VkGeometryNV> geometries, std::vector<VkGeometryInstance> instances)
{
    _memoryManager->CreateScratchBuffer(_bot,_top, _scratchBuffer);
    VkCommandBuffer commandBuffer = _commandBufferManager->BeginCommandBufferInstance();

    VkMemoryBarrier memoryBarrier;
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;

    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
        asInfo.flags = 0;
        asInfo.instanceCount = 0;
        asInfo.geometryCount = (uint32_t)geometries.size();
        asInfo.pGeometries = &geometries[0];

        _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, VK_NULL_HANDLE, 0, VK_FALSE, _bot, VK_NULL_HANDLE, _scratchBuffer.buffer , 0);
    }
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
    
    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
        asInfo.flags = 0;
        asInfo.instanceCount = 1;
        asInfo.geometryCount = 0;
        asInfo.pGeometries = nullptr;
        _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, _instanceBuffer.buffer, 0, VK_FALSE, _top, VK_NULL_HANDLE, _scratchBuffer.buffer, 0);
    }
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    vkQueueSubmit(_instance->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_instance->GetGraphicsQueue());
}

AccelerationStructure::AccelerationStructure(Instance *instance, DeviceMemoryManager *deviceMemoryManager, CommandBufferManager *commandBufferManager)
{
    _rtUtil = RTUtilities::GetInstance();
    _instance = instance;
    _memoryManager = deviceMemoryManager; 
    _commandBufferManager = commandBufferManager;
    std::vector<VkGeometryNV> geometries;
    {
        struct Vertex
        {
            float X, Y, Z;
        };

        std::vector<Vertex> vertices
        {
            { -0.5f, -0.5f, 0.0f },
            { +0.0f, +0.5f, 0.0f },
            { +0.5f, -0.5f, 0.0f }
        };
        const uint32_t vertexCount = (uint32_t)vertices.size();
        const VkDeviceSize vertexSize = sizeof(Vertex);
        const VkDeviceSize vertexBufferSize = vertexCount * vertexSize;

        std::vector<uint16_t> indices
        {
            { 0, 1, 2 }
        };
        const uint32_t indexCount = (uint32_t)indices.size();
        const VkDeviceSize indexSize = sizeof(uint16_t);
        const VkDeviceSize indexBufferSize = indexCount * indexSize;

        _memoryManager->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, MemProps::HOST, vertexBufferSize, _vertexBuffer);
        _memoryManager->CopyDataToBuffer(_vertexBuffer, vertices.data());

        _memoryManager->CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, MemProps::HOST, indexBufferSize, _indexBuffer);
        _memoryManager->CopyDataToBuffer(_indexBuffer, indices.data());

        VkGeometryNV geometry;
        geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
        geometry.pNext = nullptr;
        geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
        geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
        geometry.geometry.triangles.pNext = nullptr;
        geometry.geometry.triangles.vertexData = _vertexBuffer.buffer;
        geometry.geometry.triangles.vertexOffset = 0;
        geometry.geometry.triangles.vertexCount = vertexCount;
        geometry.geometry.triangles.vertexStride = vertexSize;
        geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        geometry.geometry.triangles.indexData = _indexBuffer.buffer;
        geometry.geometry.triangles.indexOffset = 0;
        geometry.geometry.triangles.indexCount = indexCount;
        geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT16;
        geometry.geometry.triangles.transformData = VK_NULL_HANDLE;
        geometry.geometry.triangles.transformOffset = 0;
        geometry.geometry.aabbs = { };
        geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
        geometry.flags = 0;

        geometries.emplace_back(geometry);
    }


    CreateBotTopStructures(geometries);

    std::vector<VkGeometryInstance> instances;
    {
        uint64_t accelerationStructureHandle;
        VkResult code = _rtUtil->vkGetAccelerationStructureHandleNV(_instance->GetDevice(), _bot, sizeof(uint64_t), &accelerationStructureHandle);

        float transform[12] =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
        };

        VkGeometryInstance instance;
        memcpy(instance.transform, transform, sizeof(instance.transform));
        instance.instanceId = 0;
        instance.mask = 0xff;
        instance.instanceOffset = 0;
        instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV;
        instance.accelerationStructureHandle = accelerationStructureHandle;

        uint32_t instanceBufferSize = (uint32_t)sizeof(VkGeometryInstance);

        _memoryManager->CreateBuffer(VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, MemProps::HOST, instanceBufferSize, _instanceBuffer);
        _memoryManager->CopyDataToBuffer(_instanceBuffer, &instance);

    }

    FillOutStructure(geometries,instances);
}

VkAccelerationStructureNV AccelerationStructure::GetTopStructure()
{
    return _top;
}

VkAccelerationStructureNV AccelerationStructure::GetBotStructure()
{
    return _bot;
}

}