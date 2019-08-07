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
    Utilities::CheckVkResult(code, "Failed to create a descriptor pool!");
}

void DescriptorManager::CreateDescriptorSetLayout()
{
    _rasterizationDescriptorSet = DescriptorSet::Create()
                                        .WithUniformBuffer("meshUBO", 1, VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_FRAGMENT_BIT)
                                        .WithUniformBuffer("globalUBO", 1, VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_FRAGMENT_BIT)
                                        .WithCombinedImageSampler("sampler", 1, VK_SHADER_STAGE_FRAGMENT_BIT)
                                        .Build(_instance, 500);
}

void DescriptorManager::CreateDescriptorSet(MeshInstance &instance, Image &texture, Buffer &globalUniformData, MeshInstanceHandle handle)
{
    auto instanceHandle = _rasterizationDescriptorSet->Allocate();
    _rasterizationDescriptorSet->UpdateUniformBuffer(instanceHandle, "meshUBO", &instance.uniformBuffer, 1);
    _rasterizationDescriptorSet->UpdateUniformBuffer(instanceHandle, "globalUBO", &globalUniformData, 1);
    _rasterizationDescriptorSet->UpdateImage(instanceHandle, "sampler", &texture, 1);
    _rasterizationDescriptorSet->UpdateSetInstance(instanceHandle);
    //_rasterizationDescriptorSet->UpdateUniformBuffer(instanceHandle, "meshUBO", &instances[i].uniformBuffer, 1);
    //_rasterizationDescriptorSet->UpdateUniformBuffer(instanceHandle, "globalUBO", &globalUniformData, 1);
    //_rasterizationDescriptorSet->UpdateImage(instanceHandle, "sampler", &textures[instances[i].texture], 1);
    //_rasterizationDescriptorSet->UpdateSetInstance(instanceHandle);
    _rasterizationHandles.insert({handle, instanceHandle});
}

void DescriptorManager::UpdateDescriptor(MeshInstanceHandle instance, Image &texture)
{
    auto handle = _rasterizationHandles[instance];
    _rasterizationDescriptorSet->UpdateImage(handle, "sampler", &texture, 1);
    _rasterizationDescriptorSet->UpdateSetInstance(handle);

}

void DescriptorManager::CreateDescriptorSetLayoutRT(uint32_t meshCount, uint32_t instanceCount, uint32_t textureCount)
{
    _raytracingDescriptorSet = DescriptorSet::Create()
                                    .WithAccelerationStructure("accelerationStructure", 1, VK_SHADER_STAGE_RAYGEN_BIT_NV | VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV)
                                    .WithStorageImage("targetImage", 1, VK_SHADER_STAGE_RAYGEN_BIT_NV)
                                    .WithUniformBuffer("globalBuffer", 1, VK_SHADER_STAGE_RAYGEN_BIT_NV | VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV | VK_SHADER_STAGE_MISS_BIT_NV)
                                    .WithUniformTexelBuffer("instanceMapping", 1, VK_FORMAT_R32_UINT, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV)
                                    .WithUniformTexelBuffer("indexBuffer", meshCount, VK_FORMAT_R16_UINT,  VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV)
                                    .WithUniformTexelBuffer("vertexBuffer", meshCount, VK_FORMAT_R32_SFLOAT, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV)
                                    .WithUniformBuffer("instanceUniform", instanceCount, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV)
                                    .WithCombinedImageSampler("textureUniform", textureCount, VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV)
                                    .Build(_instance, 1000U);
    //std::cout << _raytracingDescriptorSet->GetBindingsInformation() << std::endl;
}

void DescriptorManager::CreateDescriptorSetRT(AccelerationStructure *AS, Image imageViewRT, Buffer &globalUniform, std::vector<MeshInfo> meshes, std::vector<MeshInstance> instances, Buffer &instanceBuffer, std::vector<Image> textures)
{
    std::vector<Buffer> vertBuffers = {};
    std::vector<Buffer> indBuffers = {};
    std::vector<Buffer> instBuffers = {};

    for(auto& mesh : meshes)
    {
        vertBuffers.push_back(mesh.vertexBuffer);
        indBuffers.push_back(mesh.indexBuffer);
    }
    
    for(auto instance : instances)
    {
        instBuffers.push_back(instance.uniformBuffer);
    }

    rtSetHandle = _raytracingDescriptorSet->Allocate();

    _raytracingDescriptorSet->UpdateAccelerationStructure(rtSetHandle, "accelerationStructure", AS);
    _raytracingDescriptorSet->UpdateImage(rtSetHandle, "targetImage", &imageViewRT, 1);
    _raytracingDescriptorSet->UpdateUniformBuffer(rtSetHandle, "globalBuffer", &globalUniform, 1);
    _raytracingDescriptorSet->UpdateUniformBuffer(rtSetHandle, "instanceMapping", &instanceBuffer, 1);
    _raytracingDescriptorSet->UpdateUniformBuffer(rtSetHandle, "indexBuffer", indBuffers.data(), indBuffers.size());
    _raytracingDescriptorSet->UpdateUniformBuffer(rtSetHandle, "vertexBuffer", vertBuffers.data(), vertBuffers.size());
    _raytracingDescriptorSet->UpdateUniformBuffer(rtSetHandle, "instanceUniform", instBuffers.data(), instBuffers.size());
    _raytracingDescriptorSet->UpdateImage(rtSetHandle, "textureUniform", textures.data(), textures.size());
    _raytracingDescriptorSet->UpdateSetInstance(rtSetHandle);
    std::cout << _raytracingDescriptorSet->GetBindingsInformation() << std::endl;

}

void DescriptorManager::UpdateRTTargetImage(Image imageViewRT)
{   
    _raytracingDescriptorSet->UpdateImage(rtSetHandle, "targetImage", &imageViewRT, 1);
    _raytracingDescriptorSet->UpdateSetInstance(rtSetHandle);
}

VkPipelineLayout DescriptorManager::GetDescriptorLayout()
{
    return _rasterizationDescriptorSet->GetPipelineLayout();
    //return &_layout;
}

VkPipelineLayout DescriptorManager::GetDescriptorLayoutRT()
{
    return _raytracingDescriptorSet->GetPipelineLayout();
    //return _layoutsRT;
}

VkDescriptorPool DescriptorManager::GetDescriptorPool()
{
    return _pool;
}

std::vector<VkDescriptorSet> DescriptorManager::GetDescriptorSets()
{
    return _rasterizationDescriptorSet->GetVkDescriptorSets();
    //return _descriptorSets;
}

std::vector<VkDescriptorSet> DescriptorManager::GetDescriptorSetRT()
{
    return _raytracingDescriptorSet->GetVkDescriptorSets();
    //return _descriptorsetsRT;
}

};