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
    for(uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
    {
        CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV, (uint32_t)geometries.size(), geometries.data(), 0, _bot[botIndex]);
    }
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

    for(uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
        asInfo.flags = 0;
        asInfo.instanceCount = 0;
        asInfo.geometryCount = 1;
        asInfo.pGeometries = &geometries[botIndex];

        _rtUtil->vkCmdBuildAccelerationStructureNV(commandBuffer, &asInfo, VK_NULL_HANDLE, 0, VK_FALSE, _bot[botIndex], VK_NULL_HANDLE, _scratchBuffer.buffer , 0);
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
    }
      
    {
        VkAccelerationStructureInfoNV asInfo;
        asInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
        asInfo.pNext = NULL;
        asInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
        asInfo.flags = 0;
        asInfo.instanceCount = instances.size();
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

AccelerationStructure::AccelerationStructure(   Instance *instance, DeviceMemoryManager *deviceMemoryManager, 
                                                CommandBufferManager *commandBufferManager, std::vector<MeshInfo*> &meshes,
                                                std::vector<MeshInstance> &meshInstances)
{
    _bot.resize(meshes.size(), VK_NULL_HANDLE);
    _rtUtil = RTUtilities::GetInstance();
    _instance = instance;
    _memoryManager = deviceMemoryManager; 
    _commandBufferManager = commandBufferManager;
    std::vector<VkGeometryNV> geometries;
    for(uint32_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
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
        geometry.geometry.aabbs = { };
        geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV;
        geometry.flags = 0;
        geometries.emplace_back(geometry);

    }
    /*{
        const float scale = 0.25f;
        const float d = (1.0f + sqrt(5.0f)) * 0.5f * scale;

        struct Vertex
        {
            float X, Y, Z;
        };
        std::vector<Vertex> vertices
        {
            // Triangle vertices
            { -0.5f, -0.5f, 0.0f },
            { +0.0f, +0.5f, 0.0f },
            { +0.5f, -0.5f, 0.0f },

            // Tutorial 07 vertices
            { -10.0f, .0f, +10.0f },
            { +10.0f, .0f, +10.0f },
            { +10.0f, .0f, -10.0f },
            { -10.0f, .0f, -10.0f },

            // Icosahedron vertices
            { -scale, +d, 0 },
            { +scale, +d, 0 },
            { -scale, -d, 0 },
            { +scale, -d, 0 },

            { 0, -scale, +d },
            { 0, +scale, +d },
            { 0, -scale, -d },
            { 0, +scale, -d },

            { +d, 0, -scale },
            { +d, 0, +scale },
            { -d, 0, -scale },
            { -d, 0, +scale },
        };

        const uint32_t vertexCount = (uint32_t)vertices.size();
        const VkDeviceSize vertexSize = sizeof(Vertex);
        const VkDeviceSize vertexBufferSize = vertexCount * vertexSize;

        std::vector<uint16_t> indices
        {
            {
                // Triangle indices
                0, 1, 2,
                // Tutorial 07 indices
                // Quad indices
                0, 1, 2, 2, 3, 0,
                // Icosahedron indices
                0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
                1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
                3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
                4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
            }
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

        // Insert single triangle
        geometry.geometry.triangles.vertexCount = 3;
        geometry.geometry.triangles.indexCount = 3;
        geometries.emplace_back(geometry);

        // Insert bottom quad, use data from same vertex/index buffers, but with offset
        geometry.geometry.triangles.vertexOffset = 3 * vertexSize; // offset in bytes
        geometry.geometry.triangles.indexOffset = 3 * indexSize; // offset in bytes
        geometry.geometry.triangles.vertexCount = 4;
        geometry.geometry.triangles.indexCount = 6;
        geometries.emplace_back(geometry);

        // Insert icosahedron, use data from same vertex/index buffers, but with offset
        geometry.geometry.triangles.vertexOffset = 7 * vertexSize; // offset in bytes
        geometry.geometry.triangles.indexOffset = 9 * indexSize; // offset in bytes
        geometry.geometry.triangles.vertexCount = 12;
        geometry.geometry.triangles.indexCount = 60;
        geometries.emplace_back(geometry);
    }*/



    for(uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
    {
        CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV, (uint32_t)geometries.size(), geometries.data(), 0, _bot[botIndex]);
    }
    //CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV, 0, nullptr, 1, _top);

    uint32_t instanceNum;

    auto FillInstance = [&](uint64_t bottomAShandle, uint32_t instanceId, float transform[12])
    {
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
        for(uint32_t botIndex = 0; botIndex < _bot.size(); botIndex++)
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

        for(uint32_t instanceIndex = 0; instanceIndex < meshInstances.size(); instanceIndex++)
        {
            MeshInstance &meshInstance = meshInstances[instanceIndex];
            glm::mat4 model;
            _memoryManager->ModifyBufferData<MeshUniformData>(meshInstance.uniformBuffer, [&](MeshUniformData *data){
                model = data->ModelMatrix;
            });
            for(uint32_t colIndex = 0; colIndex < 4; colIndex++)
            {
                for(uint32_t rowIndex = 0; rowIndex < 3; rowIndex++)
                {
                    transform[(rowIndex)*4+colIndex] = model[colIndex][rowIndex];
                }    
            }
            instances.push_back(FillInstance(accelerationStructureHandles[meshInstance.mesh], instanceIndex, transform));
        }

        /*// Insert 3 instances of the bottom level AS #1
        instances.push_back(FillInstance(accelerationStructureHandles[0], 0, transform));
        transform[3] = 1.5f; // move geometry along X axis
        transform[11] = 0.5f; // move geometry along Z axis
        instances.push_back(FillInstance(accelerationStructureHandles[0], 1, transform));
        transform[3] = -1.5f;
        transform[11] = -0.5f;
        instances.push_back(FillInstance(accelerationStructureHandles[0], 2, transform));

        // Insert 1 instance of the bottom level AS #2
        float transform2[12] =
        {
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, -9.0f,
            0.0f, 0.0f, 2.0f, 0.0f,
        };
        instances.push_back(FillInstance(accelerationStructureHandles[1], 3, transform2));

        // Insert 1 instance of the bottom level AS #3
        transform[3] = 3.0f;
        transform[11] = 0.5f;
        instances.push_back(FillInstance(accelerationStructureHandles[2], 4, transform));
        */
        instanceNum = (uint32_t)instances.size();

        uint32_t instanceBufferSize = instanceNum * sizeof(VkGeometryInstance);

        _memoryManager->CreateBuffer(VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, MemProps::HOST, instanceBufferSize, _instanceBuffer);
        _memoryManager->CopyDataToBuffer(_instanceBuffer, instances.data());

    }
    CreateStructure(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV, 0, nullptr, instanceNum, _top);
    FillOutStructure(geometries,instances);
}

VkAccelerationStructureNV AccelerationStructure::GetTopStructure()
{
    return _top;
}

std::vector<VkAccelerationStructureNV> AccelerationStructure::GetBotStructures()
{
    return _bot;
}

}