#include "DescriptorManager.hpp"

#include "Utilities.h"

#include <array>


namespace RTE::Rendering
{

DescriptorManager::DescriptorManager(Instance *instance)
{
    _instance = instance;
}

void DescriptorManager::CreateDescriptorPools(SwapChain *swapChain, std::vector<MeshInstance> &instances)
{
    size_t poolsCount = instances.size();
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

        VkResult code = vkCreateDescriptorPool(_instance->GetDevice(), &poolInfo, nullptr, &_pools[poolIndex]);
        Utilities::CheckVkResult(code, "Failed to create descriptor pool!");
    }

    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * 11;
    pool_info.poolSizeCount = (uint32_t)11;
    pool_info.pPoolSizes = pool_sizes;
    VkResult code = vkCreateDescriptorPool(_instance->GetDevice(), &pool_info, nullptr, &_pool);
    Utilities::CheckVkResult(code, "Failed to create descriptor pool!");
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

    VkResult code = vkCreateDescriptorSetLayout(_instance->GetDevice(), &layoutInfo, nullptr, &_layout);
    Utilities::CheckVkResult(code, "DescriptorManager: Failed to create descriptor set layout!");

}

void DescriptorManager::CreateDescriptorSetLayoutRT(uint32_t meshCount, uint32_t instanceCount, uint32_t textureCount)
{
    _layoutsRT.resize(4);
    VkDescriptorSetLayoutBinding accelerationStructureLayoutBinding = {};
    accelerationStructureLayoutBinding.binding = 0;
    accelerationStructureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
    accelerationStructureLayoutBinding.descriptorCount = 1;
    accelerationStructureLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV | VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
    accelerationStructureLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding outputImageLayoutBinding = {};
    outputImageLayoutBinding.binding = 1;
    outputImageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputImageLayoutBinding.descriptorCount = 1;
    outputImageLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV;
    outputImageLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding globalDataBinding = {};
    globalDataBinding.binding = 2;
    globalDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalDataBinding.descriptorCount = 1;
    globalDataBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV | VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV | VK_SHADER_STAGE_MISS_BIT_NV;
    globalDataBinding.pImmutableSamplers = nullptr;

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
        std::vector<VkDescriptorSetLayoutBinding> bindings({ accelerationStructureLayoutBinding, outputImageLayoutBinding, globalDataBinding, instanceMappingBinding, indexBuffersBinding });
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
    Utilities::CheckVkResult(code, "Failed to create instance descriptor set layout (RT)!");

    VkDescriptorSetLayoutBinding textureUniformLayout = {};
    textureUniformLayout.binding = 4;
    textureUniformLayout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureUniformLayout.descriptorCount = textureCount;
    textureUniformLayout.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
    layoutInfo.pBindings = &textureUniformLayout;
    int i = 5;
    code = vkCreateDescriptorSetLayout(_instance->GetDevice(), &layoutInfo, nullptr, &_layoutsRT[3]);
    Utilities::CheckVkResult(code, "Failed to create texture descriptor set layout (RT)!");
}

void DescriptorManager::CreateDescriptorSetRT(AccelerationStructure *AS, VkImageView imageViewRT, BufferInformation &globalUniform, std::vector<MeshInfo> meshes, std::vector<MeshInstance> instances, BufferInformation &instanceBuffer, std::vector<TextureInfo> textures)
{
    _descriptorsetsRT.resize(4);


    std::vector<VkDescriptorPoolSize> poolSizes
    ({
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
        { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, 1 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, (uint32_t)meshes.size() * 3 + 1},
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (uint32_t)instances.size() },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (uint32_t)textures.size() },
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

    uint32_t variableDescriptorCounts[4] = 
    {
        (uint32_t)meshes.size(),
        (uint32_t)meshes.size(),
        (uint32_t)instances.size(),
        (uint32_t)textures.size(),
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

    auto& topStructure = AS->GetTopStructure();

    _descriptorAccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
    _descriptorAccelerationStructureInfo.pNext = nullptr;
    _descriptorAccelerationStructureInfo.accelerationStructureCount = 1;
    _descriptorAccelerationStructureInfo.pAccelerationStructures = &topStructure;

    VkWriteDescriptorSet accelerationStructureWrite = {};
    accelerationStructureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    accelerationStructureWrite.pNext = &_descriptorAccelerationStructureInfo; // Notice that pNext is assigned here!
    accelerationStructureWrite.dstSet = _descriptorsetsRT[0];
    accelerationStructureWrite.dstBinding = 0;
    accelerationStructureWrite.dstArrayElement = 0;
    accelerationStructureWrite.descriptorCount = 1;
    accelerationStructureWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
    accelerationStructureWrite.pImageInfo = nullptr;
    accelerationStructureWrite.pBufferInfo = nullptr;
    accelerationStructureWrite.pTexelBufferView = nullptr;


    _descriptorOutputImageInfo.sampler = nullptr;
    _descriptorOutputImageInfo.imageView = imageViewRT;
    _descriptorOutputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet outputImageWrite = {};
    outputImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    outputImageWrite.pNext = nullptr;
    outputImageWrite.dstSet = _descriptorsetsRT[0];
    outputImageWrite.dstBinding = 1;
    outputImageWrite.dstArrayElement = 0;
    outputImageWrite.descriptorCount = 1;
    outputImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputImageWrite.pImageInfo = &_descriptorOutputImageInfo;
    outputImageWrite.pBufferInfo = nullptr;
    outputImageWrite.pTexelBufferView = nullptr;


    _globalBuffer.buffer = globalUniform.buffer;
    _globalBuffer.offset = 0;
    _globalBuffer.range = sizeof(GlobalUniformData);

    VkWriteDescriptorSet globalUniformWrite = {}; //<--------------- FUNNY MOMENT. BROKE WITHOUT THE "= {}" #RIP
    globalUniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    globalUniformWrite.dstSet = _descriptorsetsRT[0];
    globalUniformWrite.dstBinding = 2;
    globalUniformWrite.dstArrayElement = 0;
    globalUniformWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalUniformWrite.descriptorCount = 1;
    globalUniformWrite.pBufferInfo = &_globalBuffer;

    VkBufferViewCreateInfo bufferViewInfo;
    bufferViewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    bufferViewInfo.pNext = nullptr;
    bufferViewInfo.flags = 0;
    bufferViewInfo.buffer = instanceBuffer.buffer;
    bufferViewInfo.format = VK_FORMAT_R32_UINT;
    bufferViewInfo.offset = 0;
    bufferViewInfo.range = VK_WHOLE_SIZE;
    code = vkCreateBufferView(_instance->GetDevice(), &bufferViewInfo, nullptr, &_mappingView);
    Utilities::CheckVkResult(code, "Could not create instance mapping buffer view!");

    VkWriteDescriptorSet instanceMappingBuffer = {};
    instanceMappingBuffer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    instanceMappingBuffer.dstSet = _descriptorsetsRT[0];
    instanceMappingBuffer.dstBinding = 3;
    instanceMappingBuffer.descriptorCount = 1;
    instanceMappingBuffer.dstArrayElement = 0;
    instanceMappingBuffer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    instanceMappingBuffer.pTexelBufferView = &_mappingView;

    _indexViews.resize(meshes.size());
    _vertexViews.resize(meshes.size());
    _meshBufferInfos.resize(instances.size());
    for(uint32_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        bufferViewInfo.buffer = meshes[meshIndex].indexBuffer.buffer;
        bufferViewInfo.format = VK_FORMAT_R16_UINT;
        code = vkCreateBufferView(_instance->GetDevice(), &bufferViewInfo, nullptr, &_indexViews[meshIndex]);
        Utilities::CheckVkResult(code, "Could not create index buffer view!");

        bufferViewInfo.buffer = meshes[meshIndex].vertexBuffer.buffer;
        bufferViewInfo.format = VK_FORMAT_R32_SFLOAT;
        code = vkCreateBufferView(_instance->GetDevice(), &bufferViewInfo, nullptr, &_vertexViews[meshIndex]);
        Utilities::CheckVkResult(code, "Could not create vertex buffer view!");
    }

    
    for(uint32_t instanceIndex = 0; instanceIndex < instances.size(); instanceIndex++)
    {
        _meshBufferInfos[instanceIndex].buffer = instances[instanceIndex].uniformBuffer.buffer;
        _meshBufferInfos[instanceIndex].offset = 0;
        _meshBufferInfos[instanceIndex].range = sizeof(MeshInstanceUniformData);
    }


    VkWriteDescriptorSet indexBuffers = {};
    indexBuffers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    indexBuffers.dstSet = _descriptorsetsRT[0];
    indexBuffers.dstBinding = 4;
    indexBuffers.descriptorCount = (uint32_t)_indexViews.size();
    indexBuffers.dstArrayElement = 0;
    indexBuffers.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    indexBuffers.pTexelBufferView = _indexViews.data();

    VkWriteDescriptorSet vertexBuffers = {};
    vertexBuffers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    vertexBuffers.dstSet = _descriptorsetsRT[1];
    vertexBuffers.dstBinding = 4;
    vertexBuffers.descriptorCount = (uint32_t)_vertexViews.size();
    vertexBuffers.dstArrayElement = 0;
    vertexBuffers.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    vertexBuffers.pTexelBufferView = _vertexViews.data();

    VkWriteDescriptorSet instanceBuffers = {};
    instanceBuffers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    instanceBuffers.dstSet = _descriptorsetsRT[2];
    instanceBuffers.dstBinding = 4;
    instanceBuffers.descriptorCount = (uint32_t)_meshBufferInfos.size();
    instanceBuffers.dstArrayElement = 0;
    instanceBuffers.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    instanceBuffers.pBufferInfo = _meshBufferInfos.data();


    _imageInfos.resize(textures.size(), {});
    for(uint32_t textureIndex = 0; textureIndex < textures.size(); textureIndex++)
    {
        _imageInfos[textureIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        _imageInfos[textureIndex].imageView = textures[textureIndex].image.imageView;
        _imageInfos[textureIndex].sampler = textures[textureIndex].sampler;
    }
    

    VkWriteDescriptorSet textureSamplers = {};
    textureSamplers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    textureSamplers.dstSet = _descriptorsetsRT[3];
    textureSamplers.dstBinding = 4;
    textureSamplers.dstArrayElement = 0;
    textureSamplers.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureSamplers.descriptorCount = (uint32_t)textures.size();
    textureSamplers.pImageInfo = _imageInfos.data();

    
    //std::vector<VkWriteDescriptorSet> descriptorWrites({ accelerationStructureWrite, outputImageWrite, cameraUniform, instanceMappingBuffer, indexBuffers, vertexBuffers, instanceBuffers });
    _descriptorWritesRT.push_back(accelerationStructureWrite);
    _descriptorWritesRT.push_back(outputImageWrite);
    _descriptorWritesRT.push_back(globalUniformWrite);
    _descriptorWritesRT.push_back(instanceMappingBuffer);
    _descriptorWritesRT.push_back(indexBuffers);
    _descriptorWritesRT.push_back(vertexBuffers);
    //In if no instances exist, include the instance decriptor set.
    if(_meshBufferInfos.size() > 0) _descriptorWritesRT.push_back(instanceBuffers);
    _descriptorWritesRT.push_back(textureSamplers);
    
    vkUpdateDescriptorSets(_instance->GetDevice(), (uint32_t)_descriptorWritesRT.size(), _descriptorWritesRT.data(), 0, nullptr);
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
        meshBuffer.range = sizeof(MeshInstanceUniformData);

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


void DescriptorManager::UpdateRTTargetImage(VkImageView imageViewRT)
{
    VkDescriptorImageInfo descriptorOutputImageInfo = {};
    descriptorOutputImageInfo.sampler = nullptr;
    descriptorOutputImageInfo.imageView = imageViewRT;
    descriptorOutputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    
    _descriptorWritesRT[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    _descriptorWritesRT[1].pNext = nullptr;
    _descriptorWritesRT[1].dstSet = _descriptorsetsRT[0];
    _descriptorWritesRT[1].dstBinding = 1;
    _descriptorWritesRT[1].dstArrayElement = 0;
    _descriptorWritesRT[1].descriptorCount = 1;
    _descriptorWritesRT[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    _descriptorWritesRT[1].pImageInfo = &descriptorOutputImageInfo;
    _descriptorWritesRT[1].pBufferInfo = nullptr;
    _descriptorWritesRT[1].pTexelBufferView = nullptr;
    vkUpdateDescriptorSets(_instance->GetDevice(), (uint32_t)_descriptorWritesRT.size(), _descriptorWritesRT.data(), 0, nullptr);
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