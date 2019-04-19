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

void DescriptorManager::CreateDescriptorSetLayoutRT()
{
    VkDescriptorSetLayoutBinding accelerationStructureLayoutBinding;
    accelerationStructureLayoutBinding.binding = 0;
    accelerationStructureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
    accelerationStructureLayoutBinding.descriptorCount = 1;
    accelerationStructureLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV;
    accelerationStructureLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding outputImageLayoutBinding;
    outputImageLayoutBinding.binding = 1;
    outputImageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputImageLayoutBinding.descriptorCount = 1;
    outputImageLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_NV;
    outputImageLayoutBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> bindings({ accelerationStructureLayoutBinding, outputImageLayoutBinding });

    VkDescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = nullptr;
    layoutInfo.flags = 0;
    layoutInfo.bindingCount = (uint32_t)(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult code = vkCreateDescriptorSetLayout(_instance->GetDevice(), &layoutInfo, nullptr, &_layoutRT);
    Utilities::CheckVkResult(code, "Failed to create descriptor set layout (RT)!");
}

void DescriptorManager::CreateDescriptorSetRT(AccelerationStructure *AS, VkImageView imageViewRT)
{
    std::vector<VkDescriptorPoolSize> poolSizes
    ({
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
        { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, 1 }
    });

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

    VkResult code = vkCreateDescriptorPool(_instance->GetDevice(), &descriptorPoolCreateInfo, nullptr, &_poolRT);
    Utilities::CheckVkResult(code, "Failed to create RT descriptor pool!");

    

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = _poolRT;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &_layoutRT;

    code = vkAllocateDescriptorSets(_instance->GetDevice(), &descriptorSetAllocateInfo, &_descriptorsetRT);
    Utilities::CheckVkResult(code, "Failed to allocate RT descriptor sets!");

    auto topStructure = AS->GetTopStructure();

    VkWriteDescriptorSetAccelerationStructureNV descriptorAccelerationStructureInfo;
    descriptorAccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
    descriptorAccelerationStructureInfo.pNext = nullptr;
    descriptorAccelerationStructureInfo.accelerationStructureCount = 1;
    descriptorAccelerationStructureInfo.pAccelerationStructures = &topStructure;

    VkWriteDescriptorSet accelerationStructureWrite;
    accelerationStructureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    accelerationStructureWrite.pNext = &descriptorAccelerationStructureInfo; // Notice that pNext is assigned here!
    accelerationStructureWrite.dstSet = _descriptorsetRT;
    accelerationStructureWrite.dstBinding = 0;
    accelerationStructureWrite.dstArrayElement = 0;
    accelerationStructureWrite.descriptorCount = 1;
    accelerationStructureWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
    accelerationStructureWrite.pImageInfo = nullptr;
    accelerationStructureWrite.pBufferInfo = nullptr;
    accelerationStructureWrite.pTexelBufferView = nullptr;


    VkDescriptorImageInfo descriptorOutputImageInfo;
    descriptorOutputImageInfo.sampler = nullptr;
    descriptorOutputImageInfo.imageView = imageViewRT;
    descriptorOutputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet outputImageWrite;
    outputImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    outputImageWrite.pNext = nullptr;
    outputImageWrite.dstSet = _descriptorsetRT;
    outputImageWrite.dstBinding = 1;
    outputImageWrite.dstArrayElement = 0;
    outputImageWrite.descriptorCount = 1;
    outputImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputImageWrite.pImageInfo = &descriptorOutputImageInfo;
    outputImageWrite.pBufferInfo = nullptr;
    outputImageWrite.pTexelBufferView = nullptr;


    std::vector<VkWriteDescriptorSet> descriptorWrites({ accelerationStructureWrite, outputImageWrite });
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

VkDescriptorSetLayout* DescriptorManager::GetDescriptorLayoutRT()
{
    return &_layoutRT;
}

VkDescriptorPool DescriptorManager::GetDescriptorPool()
{
    return _pool;
}

std::vector<VkDescriptorSet> DescriptorManager::GetDescriptorSets()
{
    return _descriptorSets;
}

VkDescriptorSet DescriptorManager::GetDescriptorSetRT()
{
    return _descriptorsetRT;
}

};