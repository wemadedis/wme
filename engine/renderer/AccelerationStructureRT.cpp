#include "AccelerationStructureRT.h"
#include "RTUtilities.h"
namespace RTE::Rendering
{

void AccelerationStructure::CreateStructure(VkAccelerationStructureTypeNV type, uint32_t geometryCount, VkGeometryNV *geometries, uint32_t instanceCount, VkAccelerationStructureNV &AS)
{
    VkAccelerationStructureCreateInfoNV accelerationStructureInfo;
    accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
    accelerationStructureInfo.pNext = nullptr;
    accelerationStructureInfo.compactedSize = 0;
    accelerationStructureInfo.info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
    accelerationStructureInfo.info.pNext = NULL;
    accelerationStructureInfo.info.type = type;
    accelerationStructureInfo.info.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV;
    accelerationStructureInfo.info.instanceCount = instanceCount;
    accelerationStructureInfo.info.geometryCount = geometryCount;
    accelerationStructureInfo.info.pGeometries = geometries;

    VkResult code = _rtUtil->vkCreateAccelerationStructureNV(_instance->GetDevice(), &accelerationStructureInfo, nullptr, &AS);
    Utilities::CheckVkResult(code, "Could not create acceleration structure!");
    _memoryManager->AllocateAccelerationStructureMemory(AS);
}

void AccelerationStructure::FillStructures()
{
    _memoryManager->CreateScratchBuffer(_botStructures, _top, _scratchBuffer);
    VkCommandBuffer commandBuffer = _commandBufferManager->BeginCommandBufferInstance();

    VkMemoryBarrier memoryBarrier;
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;

    for (uint32_t botIndex = 0; botIndex < _botStructures.size(); botIndex++)
    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
        asInfo.flags = 0;
        asInfo.instanceCount = 0;
        asInfo.geometryCount = 1;
        asInfo.pGeometries = &_geometries[botIndex];

        _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, VK_NULL_HANDLE, 0, VK_FALSE, _botStructures[botIndex], VK_NULL_HANDLE, _scratchBuffer.buffer, 0);
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
    }

    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
        asInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV;
        asInfo.instanceCount = (uint32_t)_instances.size();
        asInfo.geometryCount = 0;
        asInfo.pGeometries = nullptr;
        _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, _instanceBuffer.buffer, 0, VK_FALSE, _top, VK_NULL_HANDLE, _scratchBuffer.buffer, 0);
    }
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
    _commandBufferManager->SubmitCommandBufferInstance(commandBuffer, _instance->GetGraphicsQueue());
}

VkGeometryNV AccelerationStructure::CreateGeometry(MeshInfo &mesh)
{
    VkGeometryNV geometry;
    geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
    geometry.pNext = nullptr;
    geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_NV;
    geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
    geometry.geometry.triangles.pNext = nullptr;
    geometry.geometry.triangles.vertexData = mesh.vertexBuffer.buffer;
    geometry.geometry.triangles.vertexOffset = 0;
    geometry.geometry.triangles.vertexCount = mesh.VertexCount;
    geometry.geometry.triangles.vertexStride = sizeof(Vertex);
    geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    geometry.geometry.triangles.indexData = mesh.indexBuffer.buffer;
    geometry.geometry.triangles.indexOffset = 0;
    geometry.geometry.triangles.indexCount = mesh.IndexCount;
    geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT16;
    geometry.geometry.triangles.transformData = VK_NULL_HANDLE;
    geometry.geometry.triangles.transformOffset = 0;
    geometry.geometry.aabbs = {};
    geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
    geometry.flags = 0;
    return geometry;
}

void AccelerationStructure::CreateBotStructures(std::vector<MeshInfo> &meshes)
{
    _geometries.resize(meshes.size());
    _botStructures.resize(meshes.size());
    _botStructureHandles.resize(meshes.size());
    for (uint32_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        //Create the geometry
        _geometries[meshIndex] = CreateGeometry(meshes[meshIndex]);
        //Create the acceleration structure for the geometry
        CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV, 1, &_geometries[meshIndex], 0, _botStructures[meshIndex]);
        //Store the handle of the structure
        VkResult code = _rtUtil->vkGetAccelerationStructureHandleNV(_instance->GetDevice(), _botStructures[meshIndex], sizeof(uint64_t), &_botStructureHandles[meshIndex]);
        Utilities::CheckVkResult(code, "Could not get the bot acceleration structure handle!");
    }

}

VkGeometryInstance AccelerationStructure::ConvertInstance(MeshInstance &instance, uint32_t instanceId)
{
    //Retrieve the model matrix
    glm::mat4 model;
    _memoryManager->ModifyBufferData<MeshInstanceUniformData>(instance.uniformBuffer, [&](MeshInstanceUniformData *data) {
        model = data->ModelMatrix;
    });

    //Create the geometry instance
    VkGeometryInstance geometryInstance;
    memcpy(geometryInstance.transform, &glm::transpose(model)[0], sizeof(geometryInstance.transform));
    geometryInstance.instanceId = instanceId;
    geometryInstance.mask = 0xff;
    geometryInstance.instanceOffset = 0;
    geometryInstance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_CULL_DISABLE_BIT_NV;
    geometryInstance.accelerationStructureHandle = _botStructureHandles[instance.mesh];
    return geometryInstance;
}

void AccelerationStructure::CreateTopStructure(std::vector<MeshInstance> &instances)
{
    //Convert instances to VkGeometryInstances
    for (uint32_t instanceIndex = 0; instanceIndex < instances.size(); instanceIndex++)
    {
        _instances.push_back(ConvertInstance(instances[instanceIndex], instanceIndex));
        _instanceMeshMapping.push_back(instances[instanceIndex].mesh);
    }
    //Create an instance buffer containing all instance data
    if (_instances.size() != 0)
    {
        _memoryManager->CreateBuffer(VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, MemProps::HOST, _instances.size() * sizeof(VkGeometryInstance), _instanceBuffer);
        _memoryManager->CopyDataToBuffer(_instanceBuffer, _instances.data());
    }
    //Create the top structure (previous steps not necessary, but related to the top structure)
    CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV, 0, nullptr, static_cast<uint32_t>(_instances.size()), _top);
    
    //Create the descriptor write 
    _descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
    _descriptorWrite.pNext = nullptr;
    _descriptorWrite.accelerationStructureCount = 1;
    _descriptorWrite.pAccelerationStructures = &_top;
}

AccelerationStructure::AccelerationStructure(Instance *instance, DeviceMemoryManager *deviceMemoryManager,
                                             CommandBufferManager *commandBufferManager, std::vector<MeshInfo> &meshes,
                                             std::vector<MeshInstance> &meshInstances)
{
    _botStructures.resize(meshes.size(), VK_NULL_HANDLE);
    _rtUtil = RTUtilities::GetInstance();
    _instance = instance;
    _memoryManager = deviceMemoryManager;
    _commandBufferManager = commandBufferManager;
    CreateBotStructures(meshes);
    CreateTopStructure(meshInstances);
    FillStructures();
}

VkAccelerationStructureNV& AccelerationStructure::GetTopStructure()
{
    return _top;
}

std::vector<VkAccelerationStructureNV> AccelerationStructure::GetBotStructures()
{
    return _botStructures;
}

VkWriteDescriptorSetAccelerationStructureNV& AccelerationStructure::GetDescriptorWrite()
{
    return _descriptorWrite;
}

void AccelerationStructure::UpdateInstanceTransform(MeshInstanceHandle instanceHandle, glm::mat4 modelMatrix)
{
    memcpy(_instances[instanceHandle].transform, &glm::transpose(modelMatrix)[0], sizeof(_instances[instanceHandle].transform));
}

void AccelerationStructure::RebuildTopStructureCmd(VkCommandBuffer commandBuffer)
{
    _memoryManager->CopyDataToBuffer(_instanceBuffer, _instances.data());
    VkAccelerationStructureInfoNV asInfo;
    asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
    asInfo.pNext = NULL;
    asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
    asInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_NV;
    asInfo.instanceCount = _instances.size();
    asInfo.geometryCount = 0;
    asInfo.pGeometries = nullptr;
    _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, _instanceBuffer.buffer, 0, VK_TRUE, _top, _top, _scratchBuffer.buffer, 0);
}

} // namespace RTE::Rendering