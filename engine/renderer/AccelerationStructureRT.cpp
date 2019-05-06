#include "AccelerationStructureRT.h"
#include "RTUtilities.h"
namespace RTE::Rendering
{

void AccelerationStructure::CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV *geometries, uint32_t instanceCount, VkAccelerationStructureNV &AS)
{
    _instanceCount = instanceCount;
    VkAccelerationStructureCreateInfoNV accelerationStructureInfo;
    accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
    accelerationStructureInfo.pNext = nullptr;
    accelerationStructureInfo.compactedSize = 0;
    accelerationStructureInfo.info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
    accelerationStructureInfo.info.pNext = NULL;
    accelerationStructureInfo.info.type = type;
    accelerationStructureInfo.info.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV;
    accelerationStructureInfo.info.instanceCount = _instanceCount;
    accelerationStructureInfo.info.geometryCount = geometryCount;
    accelerationStructureInfo.info.pGeometries = geometries;

    VkResult code = _rtUtil->vkCreateAccelerationStructureNV(_instance->GetDevice(), &accelerationStructureInfo, nullptr, &AS);
    Utilities::CheckVkResult(code, "Could not create acceleration structure!");
    _memoryManager->AllocateAccelerationStructureMemory(AS);
}

void AccelerationStructure::CreateBotTopStructures(std::vector<VkGeometryNV> geometries)
{
    for (uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
    {
        CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV, (uint32_t)geometries.size(), geometries.data(), 0, _bot[botIndex]);
    }
    CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV, 0, nullptr, 1, _top);
}

void AccelerationStructure::FillOutStructure(std::vector<VkGeometryNV> geometries, std::vector<VkGeometryInstance> instances)
{
    _memoryManager->CreateScratchBuffer(_bot, _top, _scratchBuffer);
    VkCommandBuffer commandBuffer = _commandBufferManager->BeginCommandBufferInstance();

    VkMemoryBarrier memoryBarrier;
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;

    for (uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
        asInfo.flags = 0;
        asInfo.instanceCount = 0;
        asInfo.geometryCount = 1;
        asInfo.pGeometries = &geometries[botIndex];

        _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, VK_NULL_HANDLE, 0, VK_FALSE, _bot[botIndex], VK_NULL_HANDLE, _scratchBuffer.buffer, 0);
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
    }

    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
        asInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV;
        asInfo.instanceCount = (uint32_t)instances.size();
        asInfo.geometryCount = 0;
        asInfo.pGeometries = nullptr;
        _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, _instanceBuffer.buffer, 0, VK_FALSE, _top, VK_NULL_HANDLE, _scratchBuffer.buffer, 0);
    }
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
    _commandBufferManager->SubmitCommandBufferInstance(commandBuffer, _instance->GetGraphicsQueue());
}

AccelerationStructure::AccelerationStructure(Instance *instance, DeviceMemoryManager *deviceMemoryManager,
                                             CommandBufferManager *commandBufferManager, std::vector<MeshInfo *> &meshes,
                                             std::vector<MeshInstance> &meshInstances)
{
    _bot.resize(meshes.size(), VK_NULL_HANDLE);
    _rtUtil = RTUtilities::GetInstance();
    _instance = instance;
    _memoryManager = deviceMemoryManager;
    _commandBufferManager = commandBufferManager;
    std::vector<VkGeometryNV> geometries;
    for (uint32_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        VkGeometryNV geometry;
        geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
        geometry.pNext = nullptr;
        geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
        geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
        geometry.geometry.triangles.pNext = nullptr;
        geometry.geometry.triangles.vertexData = meshes[meshIndex]->vertexBuffer.buffer;
        geometry.geometry.triangles.vertexOffset = 0;
        geometry.geometry.triangles.vertexCount = meshes[meshIndex]->VertexCount;
        geometry.geometry.triangles.vertexStride = sizeof(Vertex);
        geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        geometry.geometry.triangles.indexData = meshes[meshIndex]->indexBuffer.buffer;
        geometry.geometry.triangles.indexOffset = 0;
        geometry.geometry.triangles.indexCount = meshes[meshIndex]->IndexCount;
        geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT16;
        geometry.geometry.triangles.transformData = VK_NULL_HANDLE;
        geometry.geometry.triangles.transformOffset = 0;
        geometry.geometry.aabbs = {};
        geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
        geometry.flags = 0;
        geometries.emplace_back(geometry);
    }

    //TODO: NOTE DOWN IN REPORT HOW THE MAPPING WORKS
    for (uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
    {
        CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV, (uint32_t)geometries.size(), geometries.data(), 0, _bot[botIndex]);
    }
    //CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV, 0, nullptr, 1, _top);

    uint32_t instanceNum;

    auto FillInstance = [&](uint64_t bottomAShandle, uint32_t instanceId, float transform[12]) {
        VkGeometryInstance instance;
        memcpy(instance.transform, transform, sizeof(instance.transform));
        instance.instanceId = instanceId;
        instance.mask = 0xff;
        instance.instanceOffset = 0;
        instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV;
        instance.accelerationStructureHandle = bottomAShandle;
        return instance;
    };

    std::vector<VkGeometryInstance> instances;
    {
        std::vector<uint64_t> accelerationStructureHandles;
        accelerationStructureHandles.resize(_bot.size());
        for (uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
        {
            VkResult code = _rtUtil->vkGetAccelerationStructureHandleNV(_instance->GetDevice(), _bot[botIndex], sizeof(uint64_t), &accelerationStructureHandles[botIndex]);
            Utilities::CheckVkResult(code, "Could not get the bot acceleration structure handle!");
        }

        float transform[12] =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
        };

        for (uint32_t instanceIndex = 0; instanceIndex < meshInstances.size(); instanceIndex++)
        {
            MeshInstance &meshInstance = meshInstances[instanceIndex];
            glm::mat4 model;
            _memoryManager->ModifyBufferData<MeshInstanceUniformData>(meshInstance.uniformBuffer, [&](MeshInstanceUniformData *data) {
                model = data->ModelMatrix;
            });
            for (uint32_t colIndex = 0; colIndex < 4; colIndex++)
            {
                for (uint32_t rowIndex = 0; rowIndex < 3; rowIndex++)
                {
                    transform[(rowIndex)*4 + colIndex] = model[colIndex][rowIndex];
                }
            }
            instances.push_back(FillInstance(accelerationStructureHandles[meshInstance.mesh], instanceIndex, transform));
            //TODO: Note down the structure here as well
            _instanceMeshMapping.push_back(meshInstance.mesh);
        }
        instanceNum = (uint32_t)instances.size();

        uint32_t instanceBufferSize = instanceNum * sizeof(VkGeometryInstance);
        if (instances.size() != 0)
        {
            _memoryManager->CreateBuffer(VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, MemProps::HOST, instanceBufferSize, _instanceBuffer);
            _memoryManager->CopyDataToBuffer(_instanceBuffer, instances.data());
        }
    }
    CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV, 0, nullptr, instanceNum, _top);
    FillOutStructure(geometries, instances);
}

VkAccelerationStructureNV& AccelerationStructure::GetTopStructure()
{
    return _top;
}

std::vector<VkAccelerationStructureNV> AccelerationStructure::GetBotStructures()
{
    return _bot;
}

void AccelerationStructure::UpdateInstanceTransform(MeshInstanceHandle instanceHandle, glm::mat4 modelMatrix)
{
    float transform[12];
    for (uint32_t colIndex = 0; colIndex < 4; colIndex++)
    {
        for (uint32_t rowIndex = 0; rowIndex < 3; rowIndex++)
        {
            transform[(rowIndex)*4 + colIndex] = modelMatrix[colIndex][rowIndex];
        }
    }
    _memoryManager->ModifyBufferData<VkGeometryInstance>(_instanceBuffer, [&](VkGeometryInstance *instances) {
        memcpy(instances[instanceHandle].transform, transform, sizeof(transform));
    });
}

void AccelerationStructure::RebuildTopStructureCmd(VkCommandBuffer commandBuffer)
{
    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;

    VkAccelerationStructureInfoNV asInfo;
    asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
    asInfo.pNext = NULL;
    asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
    asInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV;
    asInfo.instanceCount = _instanceCount;
    asInfo.geometryCount = 0;
    asInfo.pGeometries = nullptr;
    //_rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, _instanceBuffer.buffer, 0, VK_FALSE, _top, VK_NULL_HANDLE, _scratchBuffer.buffer, 0);
    _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, _instanceBuffer.buffer, 0, VK_TRUE, _top, _top, _scratchBuffer.buffer, 0);
    //TODO: Find out why we do memory barriers here (COMMENTED OUT, NOTHING CHANGED)
    //vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
}

} // namespace RTE::Rendering