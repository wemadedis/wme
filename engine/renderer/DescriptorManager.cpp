#include "DescriptorManager.hpp"

#include "Utilities.h"

#include <array>


namespace RTE::Rendering
{

DescriptorManager::DescriptorManager(Instance *instance)
{
    _instance = instance;
}

void DescriptorManager::CreateDescriptorPool(SwapChain *swapChain, std::vector<MeshInstance> &meshes)
{
    size_t poolsCount = meshes.size();
    _pools.resize(poolsCount);
    for(uint32_t poolIndex = 0; poolIndex < poolsCount; poolIndex++)
    {
        std::array<VkDescriptorPoolSize, 3> poolSizes = {};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());

        if (vkCreateDescriptorPool(_instance->GetDevice(), &poolInfo, nullptr, &_pools[poolIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("DescriptorManager: Failed to create descriptor pool!");
        }
    }
    /*std::array<VkDescriptorPoolSize, 3> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChain->GetSwapChainImageCount());

    if (vkCreateDescriptorPool(_instance->GetDevice(), &poolInfo, nullptr, &_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("DescriptorManager: Failed to create descriptor pool!");
    }*/
}

void DescriptorManager::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding meshUboLayout = {};
    meshUboLayout.binding = 0;
    meshUboLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    meshUboLayout.descriptorCount = 1;
    meshUboLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    meshUboLayout.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding globalUboLayout = {};
    globalUboLayout.binding = 1;
    globalUboLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalUboLayout.descriptorCount = 1;
    globalUboLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    globalUboLayout.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 2;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {meshUboLayout, globalUboLayout, samplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(_instance->GetDevice(), &layoutInfo, nullptr, &_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("DescriptorManager: Failed to create descriptor set layout!");
    }
}

void DescriptorManager::CreateDescriptorSetLayoutRT(uint32_t meshCount, uint32_t instanceCount)
{
    _layoutsRT.resize(3);
    VkDescriptorSetLayoutBinding accelerationStructureLayoutBinding = {};
    accelerationStructureLayoutBinding.binding = 0;
    accelerationStructureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
    accelerationStructureLayoutBinding.descriptorCount = 1;
    accelerationStructureLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV;
    accelerationStructureLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding outputImageLayoutBinding = {};
    outputImageLayoutBinding.binding = 1;
    outputImageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputImageLayoutBinding.descriptorCount = 1;
    outputImageLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV;
    outputImageLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding cameraDataBinding = {};
    cameraDataBinding.binding = 2;
    cameraDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraDataBinding.descriptorCount = 1;
    cameraDataBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV | VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
    cameraDataBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding instanceMappingBinding = {};
    instanceMappingBinding.binding = 3;
    instanceMappingBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    instanceMappingBinding.descriptorCount = 1;
    instanceMappingBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;

    VkDescriptorSetLayoutBinding indexBuffersBinding = {};
    indexBuffersBinding.binding = 4;
    indexBuffersBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    indexBuffersBinding.descriptorCount = meshCount;
    indexBuffersBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;

    {
        std::vector<VkDescriptorSetLayoutBinding> bindings({ accelerationStructureLayoutBinding, outputImageLayoutBinding, cameraDataBinding, instanceMappingBinding, indexBuffersBinding });
        std::array<VkDescriptorBindingFlagsEXT, 5> flags = { 0, 0, 0, 0, VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT };

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlags;
        bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
        bindingFlags.pNext = nullptr;
        bindingFlags.pBindingFlags = flags.data();
        bindingFlags.bindingCount = (uint32_t)flags.size();

        VkDescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pNext = &bindingFlags;
        layoutInfo.flags = 0;
        layoutInfo.bindingCount = (uint32_t)(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VkResult code = vkCreateDescriptorSetLayout(_instance->GetDevice(), &layoutInfo, nullptr, &_layoutsRT[0]);
        Utilities::CheckVkResult(code, "Failed to create descriptor set layout (RT)!");
    }
    //The vertex buffer binding need to be created in another set layout because of the dynamic size (there can be only one per layout)
    //also, to get the variable descriptor size, we need to enable an extension when creating the logical device!
    VkDescriptorSetLayoutBinding vertexBufferBinding = {};
    vertexBufferBinding.binding = 4;
    vertexBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    vertexBufferBinding.descriptorCount = meshCount;
    vertexBufferBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;

    VkDescriptorBindingFlagsEXT flag = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlags;
    bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    bindingFlags.pNext = nullptr;
    bindingFlags.pBindingFlags = &flag;
    bindingFlags.bindingCount = 1;

    VkDescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = &bindingFlags;
    layoutInfo.flags = 0;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &vertexBufferBinding;

    VkResult code = vkCreateDescriptorSetLayout(_instance->GetDevice(), &layoutInfo, nullptr, &_layoutsRT[1]);
    Utilities::CheckVkResult(code, "Failed to create vertex descriptor set layout (RT)!");

    VkDescriptorSetLayoutBinding instanceUniformLayout = {};
    instanceUniformLayout.binding = 4;
    instanceUniformLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    instanceUniformLayout.descriptorCount = instanceCount;
    instanceUniformLayout.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;

    layoutInfo.pBindings = &instanceUniformLayout;

    code = vkCreateDescriptorSetLayout(_instance->GetDevice(), &layoutInfo, nullptr, &_layoutsRT[2]);
    Utilities::CheckVkResult(code, "Failed to create vertex descriptor set layout (RT)!");
}

void DescriptorManager::CreateDescriptorSetRT(AccelerationStructure *AS, VkImageView imageViewRT, BufferInformation &globalUniform, std::vector<MeshInfo*> meshes, std::vector<MeshInstance> instances, BufferInformation &instanceBuffer)
{
    _descriptorsetsRT.resize(3);


    std::vector<VkDescriptorPoolSize> poolSizes
    ({
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
        { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, 1 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, (uint32_t)meshes.size() },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, (uint32_t)meshes.size() },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (uint32_t)instances.size() },
    });

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = (uint32_t)_layoutsRT.size();
    descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

    VkResult code = vkCreateDescriptorPool(_instance->GetDevice(), &descriptorPoolCreateInfo, nullptr, &_poolRT);
    Utilities::CheckVkResult(code, "Failed to create RT descriptor pool!");

    uint32_t variableDescriptorCounts[3] = 
    {
        (uint32_t)meshes.size(),
        (uint32_t)meshes.size(),
        (uint32_t)instances.size(),
    };

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT variableDescriptorCountInfo;
    variableDescriptorCountInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    variableDescriptorCountInfo.pNext = nullptr;
    variableDescriptorCountInfo.descriptorSetCount = (uint32_t)_layoutsRT.size();
    variableDescriptorCountInfo.pDescriptorCounts = variableDescriptorCounts; // actual number of descriptors

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = &variableDescriptorCountInfo;
    descriptorSetAllocateInfo.descriptorPool = _poolRT;
    descriptorSetAllocateInfo.descriptorSetCount = (uint32_t)_layoutsRT.size();
    descriptorSetAllocateInfo.pSetLayouts = _layoutsRT.data();

    code = vkAllocateDescriptorSets(_instance->GetDevice(), &descriptorSetAllocateInfo, _descriptorsetsRT.data());
    Utilities::CheckVkResult(code, "Failed to allocate RT descriptor sets!");

    auto topStructure = AS->GetTopStructure();

    VkWriteDescriptorSetAccelerationStructureNV descriptorAccelerationStructureInfo = {};
    descriptorAccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
    descriptorAccelerationStructureInfo.pNext = nullptr;
    descriptorAccelerationStructureInfo.accelerationStructureCount = 1;
    descriptorAccelerationStructureInfo.pAccelerationStructures = &topStructure;

    VkWriteDescriptorSet accelerationStructureWrite = {};
    accelerationStructureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    accelerationStructureWrite.pNext = &descriptorAccelerationStructureInfo; // Notice that pNext is assigned here!
    accelerationStructureWrite.dstSet = _descriptorsetsRT[0];
    accelerationStructureWrite.dstBinding = 0;
    accelerationStructureWrite.dstArrayElement = 0;
    accelerationStructureWrite.descriptorCount = 1;
    accelerationStructureWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
    accelerationStructureWrite.pImageInfo = nullptr;
    accelerationStructureWrite.pBufferInfo = nullptr;
    accelerationStructureWrite.pTexelBufferView = nullptr;


    VkDescriptorImageInfo descriptorOutputImageInfo = {};
    descriptorOutputImageInfo.sampler = nullptr;
    descriptorOutputImageInfo.imageView = imageViewRT;
    descriptorOutputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet outputImageWrite = {};
    outputImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    outputImageWrite.pNext = nullptr;
    outputImageWrite.dstSet = _descriptorsetsRT[0];
    outputImageWrite.dstBinding = 1;
    outputImageWrite.dstArrayElement = 0;
    outputImageWrite.descriptorCount = 1;
    outputImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputImageWrite.pImageInfo = &descriptorOutputImageInfo;
    outputImageWrite.pBufferInfo = nullptr;
    outputImageWrite.pTexelBufferView = nullptr;


    VkDescriptorBufferInfo globalBuffer = {};
    globalBuffer.buffer = globalUniform.buffer;
    globalBuffer.offset = 0;
    globalBuffer.range = sizeof(GlobalUniformData);

    VkWriteDescriptorSet cameraUniform = {}; //<--------------- FUNNY MOMENT. BROKE WITHOUT THE "= {}" #RIP
    cameraUniform.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    cameraUniform.dstSet = _descriptorsetsRT[0];
    cameraUniform.dstBinding = 2;
    cameraUniform.dstArrayElement = 0;
    cameraUniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraUniform.descriptorCount = 1;
    cameraUniform.pBufferInfo = &globalBuffer;

    VkBufferViewCreateInfo bufferViewInfo;
    bufferViewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    bufferViewInfo.pNext = nullptr;
    bufferViewInfo.flags = 0;
    bufferViewInfo.buffer = instanceBuffer.buffer;
    bufferViewInfo.format = VK_FORMAT_R32_UINT;
    bufferViewInfo.offset = 0;
    bufferViewInfo.range = VK_WHOLE_SIZE;
    VkBufferView mappingView;
    code = vkCreateBufferView(_instance->GetDevice(), &bufferViewInfo, nullptr, &mappingView);
    Utilities::CheckVkResult(code, "Could not create instance mapping buffer view!");

    VkWriteDescriptorSet instanceMappingBuffer = {};
    instanceMappingBuffer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    instanceMappingBuffer.dstSet = _descriptorsetsRT[0];
    instanceMappingBuffer.dstBinding = 3;
    instanceMappingBuffer.descriptorCount = 1;
    instanceMappingBuffer.dstArrayElement = 0;
    instanceMappingBuffer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    instanceMappingBuffer.pTexelBufferView = &mappingView;

    std::vector<VkBufferView> indexViews(meshes.size());
    std::vector<VkBufferView> vertexViews(meshes.size());
    for(uint32_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        bufferViewInfo.buffer = meshes[meshIndex]->indexBuffer.buffer;
        bufferViewInfo.format = VK_FORMAT_R16_UINT;
        code = vkCreateBufferView(_instance->GetDevice(), &bufferViewInfo, nullptr, &indexViews[meshIndex]);
        Utilities::CheckVkResult(code, "Could not create index buffer view!");

        bufferViewInfo.buffer = meshes[meshIndex]->vertexBuffer.buffer;
        bufferViewInfo.format = VK_FORMAT_R32_SFLOAT;
        code = vkCreateBufferView(_instance->GetDevice(), &bufferViewInfo, nullptr, &vertexViews[meshIndex]);
        Utilities::CheckVkResult(code, "Could not create vertex buffer view!");
    }

    std::vector<VkDescriptorBufferInfo> meshBufferInfos(instances.size());
    for(uint32_t instanceIndex = 0; instanceIndex < instances.size(); instanceIndex++)
    {
        meshBufferInfos[instanceIndex].buffer = instances[instanceIndex].uniformBuffer.buffer;
        meshBufferInfos[instanceIndex].offset = 0;
        meshBufferInfos[instanceIndex].range = sizeof(MeshUniformData);
    }


    VkWriteDescriptorSet indexBuffers = {};
    indexBuffers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    indexBuffers.dstSet = _descriptorsetsRT[0];
    indexBuffers.dstBinding = 4;
    indexBuffers.descriptorCount = (uint32_t)indexViews.size();
    indexBuffers.dstArrayElement = 0;
    indexBuffers.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    indexBuffers.pTexelBufferView = indexViews.data();

    VkWriteDescriptorSet vertexBuffers = {};
    vertexBuffers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    vertexBuffers.dstSet = _descriptorsetsRT[1];
    vertexBuffers.dstBinding = 4;
    vertexBuffers.descriptorCount = (uint32_t)vertexViews.size();
    vertexBuffers.dstArrayElement = 0;
    vertexBuffers.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    vertexBuffers.pTexelBufferView = vertexViews.data();

    VkWriteDescriptorSet instanceBuffers = {};
    instanceBuffers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    instanceBuffers.dstSet = _descriptorsetsRT[2];
    instanceBuffers.dstBinding = 4;
    instanceBuffers.descriptorCount = (uint32_t)meshBufferInfos.size();
    instanceBuffers.dstArrayElement = 0;
    instanceBuffers.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    instanceBuffers.pBufferInfo = meshBufferInfos.data();


    std::vector<VkWriteDescriptorSet> descriptorWrites({ accelerationStructureWrite, outputImageWrite, cameraUniform, instanceMappingBuffer, indexBuffers, vertexBuffers, instanceBuffers });

    vkUpdateDescriptorSets(_instance->GetDevice(), (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void DescriptorManager::CreateDescriptorSets(std::vector<MeshInstance> &instances, std::vector<TextureInfo> textures, BufferInformation &globalUniformData)
{
    size_t setCount = instances.size();
    std::vector<VkDescriptorSetLayout> layouts(setCount, _layout);
    _descriptorSets.resize(setCount);
    for (size_t i = 0; i < setCount; i++) {
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _pools[i];
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layouts[i];


        VkResult result = vkAllocateDescriptorSets(_instance->GetDevice(), &allocInfo, &_descriptorSets[i]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("DescriptorManager: Failed to allocate descriptor sets!");
        }
    
    
        VkDescriptorBufferInfo meshBuffer = {};
        meshBuffer.buffer = instances[i].uniformBuffer.buffer;
        meshBuffer.offset = 0;
        meshBuffer.range = sizeof(MeshUniformData);

        VkDescriptorBufferInfo globalBuffer = {};
        globalBuffer.buffer = globalUniformData.buffer;
        globalBuffer.offset = 0;
        globalBuffer.range = sizeof(GlobalUniformData);


        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textures[instances[i].texture].image.imageView;
        imageInfo.sampler = textures[instances[i].texture].sampler;
        std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &meshBuffer;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &globalBuffer;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = _descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(_instance->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

VkDescriptorSetLayout* DescriptorManager::GetDescriptorLayout()
{
    return &_layout;
}

std::vector<VkDescriptorSetLayout> DescriptorManager::GetDescriptorLayoutRT()
{
    return _layoutsRT;
}

VkDescriptorPool DescriptorManager::GetDescriptorPool()
{
    return _pool;
}

std::vector<VkDescriptorSet> DescriptorManager::GetDescriptorSets()
{
    return _descriptorSets;
}

std::vector<VkDescriptorSet> DescriptorManager::GetDescriptorSetRT()
{
    return _descriptorsetsRT;
}

};