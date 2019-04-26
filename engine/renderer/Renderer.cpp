#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include "RenderLogicStructs.h"
#include "rte/Renderer.h"

#include "Instance.hpp"
#include "RTUtilities.h"

#include "rte/UtilityMain.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace RTE::Rendering
{
TextureHandle Renderer::EMPTY_TEXTURE;
const int MAX_FRAMES_IN_FLIGHT = 2;

void Renderer::FrameResized(int32_t width, int32_t height)
{
    _frameWidth = width;
    _frameHeight = height;
    _frameChanged = true;
}

glm::ivec2 Renderer::GetFrameSize()
{
    return { _frameWidth, _frameHeight };
}

void Renderer::Initialize()
{
    _instance = new Instance(_initInfo.extensions, _initInfo.BindingFunc, _initInfo.RayTracingOn);
    RTXon = _instance->IsRayTracingCapable();

    _swapChain = new SwapChain(_instance, _initInfo.Width, _initInfo.Height);
    _renderPass = new RenderPass(_instance, _swapChain);

    _commandBufferManager = new CommandBufferManager(_instance, (uint32_t)_swapChain->GetSwapChainImageCount());
    _deviceMemoryManager = new DeviceMemoryManager(_instance, _commandBufferManager);
    _imageManager = new ImageManager(_instance, _commandBufferManager, _deviceMemoryManager);
    _swapChain->CreateFramebuffers(_renderPass, _imageManager);

    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, MemProps::HOST, sizeof(GlobalUniformData), _globalUniformBuffer);
    CreateEmptyTexture();
}

MeshHandle Renderer::UploadMesh(Mesh &mesh)
{
    MeshInfo *info = new MeshInfo();
    info->IndexCount = mesh.Indices.size();
    info->VertexCount = mesh.Vertices.size();
    size_t bufferSize = (size_t)(sizeof(mesh.Indices[0]) * info->IndexCount);

    if (bufferSize == 0)
    {
        throw std::invalid_argument("Renderer: Trying to create buffer with size 0");
    }

    //Indices
    BufferInformation stagingBuffer = {};
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemProps::HOST, bufferSize, stagingBuffer);
    _deviceMemoryManager->CopyDataToBuffer(stagingBuffer, (void *)mesh.Indices.data());
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, MemProps::DEVICE, bufferSize, info->indexBuffer);
    _deviceMemoryManager->CopyBuffer(stagingBuffer, info->indexBuffer, bufferSize, _commandBufferManager->GetCommandPool(), _instance->GetGraphicsQueue());
    _deviceMemoryManager->DestroyBuffer(stagingBuffer);
    //Vertices
    bufferSize = sizeof(mesh.Vertices[0]) * mesh.Vertices.size();
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemProps::HOST, bufferSize, stagingBuffer);
    _deviceMemoryManager->CopyDataToBuffer(stagingBuffer, (void *)mesh.Vertices.data());
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, MemProps::DEVICE, bufferSize, info->vertexBuffer);
    _deviceMemoryManager->CopyBuffer(stagingBuffer, info->vertexBuffer, bufferSize, _commandBufferManager->GetCommandPool(), _instance->GetGraphicsQueue());

    _deviceMemoryManager->DestroyBuffer(stagingBuffer);

    _meshes.push_back(info);
    return (MeshHandle)_meshes.size() - 1;
}

MeshInstanceHandle Renderer::CreateMeshInstance(MeshHandle mesh)
{
    MeshInstance instance = {};
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, MemProps::HOST, sizeof(MeshUniformData), instance.uniformBuffer);

    MeshUniformData meshUniform = {};
    glm::mat4 rot = glm::eulerAngleXYZ(0.0f, 0.0f, 0.0f);
    glm::mat4 trn = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 scl = glm::scale(glm::vec3(1.0f));
    meshUniform.ModelMatrix = trn * rot * scl;
    instance.texture = Renderer::EMPTY_TEXTURE;
    instance.mesh = mesh;
    meshUniform.HasTexture = false;

    _deviceMemoryManager->CopyDataToBuffer(instance.uniformBuffer, &meshUniform);
    _meshInstances.push_back(instance);
    return (MeshInstanceHandle)_meshInstances.size() - 1;
}

void Renderer::BindMeshToInstance(MeshHandle mesh, MeshInstanceHandle instance)
{
    _meshInstances[instance].mesh = mesh;
}

TextureHandle Renderer::UploadTexture(Texture &texture)
{
    _textures.push_back(_imageManager->CreateTexture(texture.Width, texture.Height, texture.Pixels, texture.Width * texture.Height * 4));
    return (TextureHandle)_textures.size() - 1;
}

void Renderer::BindTextureToMeshInstance(TextureHandle texture, MeshInstanceHandle instance)
{
    _meshInstances[instance].texture = texture;
    _deviceMemoryManager->ModifyBufferData<MeshUniformData>(_meshInstances[instance].uniformBuffer, [](MeshUniformData *data) {
        data->HasTexture = true;
    });
}

void Renderer::RecordRenderPass()
{
    for (uint32_t bufferIndex = 0; bufferIndex < _commandBufferManager->GetCommandBufferCount(); bufferIndex++)
    {
        VkCommandBuffer cmdBuffer = _commandBufferManager->GetCommandBuffer(bufferIndex);
        _renderPass->BeginRenderPass(_pipeline, cmdBuffer, _swapChain->GetFramebuffers()[bufferIndex], _globalUniform.ClearColor);

        for (unsigned int meshIndex = 0; meshIndex < _meshInstances.size(); meshIndex++)
        {
            MeshInfo *mesh = _meshes[_meshInstances[meshIndex].mesh];
            VkBuffer vertexBuffers[] = {mesh->vertexBuffer.buffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(cmdBuffer, mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->GetLayout(), 0, 1, &_descriptorManager->GetDescriptorSets()[meshIndex], 0, nullptr);

            vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(mesh->IndexCount), 1, 0, 0, 0);
        }

        _renderPass->EndRenderPass(cmdBuffer);
    }
}

void Renderer::RecordCommandBufferForFrame(VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
    _accelerationStructure->RebuildTopStructureCmd(commandBuffer);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, _pipelineRT->GetHandle());
    auto dset = _descriptorManager->GetDescriptorSetRT();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, _pipelineRT->GetLayout(), 0, (uint32_t)dset.size(), dset.data(), 0, 0);

    // Here's how the shader binding table looks like in this tutorial:
    // |[ raygen shader ]|
    // |                 |
    // | 0               | 1

    RTUtilities::GetInstance()->vkCmdTraceRaysNV(commandBuffer,
                                                 _shaderBindingTable.buffer, 0,
                                                 _shaderBindingTable.buffer, 2 * _rtProperties.shaderGroupHandleSize, _rtProperties.shaderGroupHandleSize,
                                                 _shaderBindingTable.buffer, 1 * _rtProperties.shaderGroupHandleSize, _rtProperties.shaderGroupHandleSize,
                                                 VK_NULL_HANDLE, 0, 0,
                                                 _initInfo.Width, _initInfo.Height, 1);
}

void Renderer::RecordCommandBuffersRT()
{
    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    for (uint32_t bufferIndex = 0; bufferIndex < _commandBufferManager->GetCommandBufferCount(); bufferIndex++)
    {
        const VkCommandBuffer commandBuffer = _commandBufferManager->GetCommandBufferRT(bufferIndex);

        VkResult code = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
        Utilities::CheckVkResult(code, "Could not begin RT command buffer!");

        _imageManager->ImageBarrier(commandBuffer, _offScreenImageRT.image, subresourceRange,
                                    0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        RecordCommandBufferForFrame(commandBuffer, bufferIndex); // user draw code

        auto swapChainImage = _swapChain->GetSwapChainImages()[bufferIndex].imageInfo.image;

        _imageManager->ImageBarrier(commandBuffer, swapChainImage, subresourceRange,
                                    0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        _imageManager->ImageBarrier(commandBuffer, _offScreenImageRT.image, subresourceRange,
                                    VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        VkImageCopy copyRegion;
        copyRegion.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        copyRegion.srcOffset = {0, 0, 0};
        copyRegion.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        copyRegion.dstOffset = {0, 0, 0};
        copyRegion.extent = {_initInfo.Width, _initInfo.Height, 1};
        vkCmdCopyImage(commandBuffer, _offScreenImageRT.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        _imageManager->ImageBarrier(commandBuffer, swapChainImage, subresourceRange,
                                    VK_ACCESS_TRANSFER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        code = vkEndCommandBuffer(commandBuffer);
        Utilities::CheckVkResult(code, "Could not end RT command buffer!");
    }
}

void Renderer::CreateSyncObjects()
{
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(_instance->GetDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_instance->GetDevice(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_instance->GetDevice(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void Renderer::CleanupSwapChain()
{
    _commandBufferManager->DeallocateCommandBuffers();
    if (RTXon)
    {
        _commandBufferManager->DeallocateCommandBuffersRT();
    }

    delete _pipeline;

    delete _renderPass;

    delete _swapChain;
}

void Renderer::RecreateSwapChain()
{
    int width = _frameWidth;
    int height = _frameHeight;

    vkDeviceWaitIdle(_instance->GetDevice());

    CleanupSwapChain();
    _commandBufferManager->AllocateCommandBuffers();
    if (RTXon)
    {
        _commandBufferManager->AllocateCommandBuffersRT();
    }

    _swapChain = new SwapChain(_instance, width, height);
    _renderPass = new RenderPass(_instance, _swapChain);
    auto vertexShader = Utilities::GetStandardVertexShader(_instance->GetDevice());
    auto fragmentShader = Utilities::GetStandardFragmentShader(_instance->GetDevice());

    _pipeline = new GraphicsPipeline(vertexShader,
                                     fragmentShader,
                                     _swapChain->GetSwapChainExtent(),
                                     _descriptorManager,
                                     _instance,
                                     _renderPass);
    _swapChain->CreateFramebuffers(_renderPass, _imageManager);
    RecordRenderPass();
}

void Renderer::UploadGlobalUniform()
{
    _globalUniform.LightCounts.x = _directionalLights.size();
    _globalUniform.LightCounts.y = _pointLights.size();
    for (uint32_t lightIndex = 0; lightIndex < 10; lightIndex++)
    {
        if (lightIndex < _directionalLights.size())
            _globalUniform.DirectionalLights[lightIndex] = _directionalLights[lightIndex];
        if (lightIndex < _pointLights.size())
        {
            _globalUniform.PointLights[lightIndex] = _pointLights[lightIndex];
        }
    }
    _deviceMemoryManager->CopyDataToBuffer(_globalUniformBuffer, &_globalUniform);
}

void Renderer::CreateEmptyTexture()
{
    unsigned char pixels[4] = {255, 255, 255, 255};
    Texture tex = {};
    tex.Height = 1;
    tex.Width = 1;
    tex.Pixels = pixels;
    Renderer::EMPTY_TEXTURE = UploadTexture(tex);
}

void Renderer::InitRT()
{
    VkDevice device = _instance->GetDevice();
    RTUtilities::GetInstance(&device);

    _rtProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;
    _rtProperties.pNext = nullptr;
    _rtProperties.maxRecursionDepth = 0;
    _rtProperties.shaderGroupHandleSize = 0;

    VkPhysicalDeviceProperties2 props;
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    props.pNext = &_rtProperties;
    props.properties = {};

    vkGetPhysicalDeviceProperties2(_instance->GetPhysicalDevice(), &props);
}

void Renderer::CreateShaderBindingTable()
{
    const uint32_t groupNum = 3; // 1 group is listed in pGroupNumbers in VkRayTracingPipelineCreateInfoNV
    const uint32_t shaderBindingTableSize = _rtProperties.shaderGroupHandleSize * groupNum;

    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemProps::HOST, shaderBindingTableSize, _shaderBindingTable);

    _deviceMemoryManager->ModifyBufferData<void *>(_shaderBindingTable, [&](void *data) {
        VkResult code = RTUtilities::GetInstance()->vkGetRayTracingShaderGroupHandlesNV(_instance->GetDevice(), _pipelineRT->GetHandle(), 0, groupNum, shaderBindingTableSize, data);
        Utilities::CheckVkResult(code, "Failed to get RT shader group handles!");
    });
}

Renderer::Renderer(RendererInitInfo info) : _minFrameTime(1.0f / info.MaxFPS)
{
    _initInfo = info;
    _frameWidth = info.Width;
    _frameHeight = info.Height;
    _lastFrameEnd = Clock::now();
    Initialize();
    if (RTXon)
    {
        _renderMode = RenderMode::RAYTRACE;
    }
    else
    {
        _renderMode = RenderMode::RASTERIZE;
    }
}

void Renderer::Finalize()
{

    _descriptorManager = new DescriptorManager(_instance);
    _descriptorManager->CreateDescriptorSetLayout();

    auto vertexShader = Utilities::GetStandardVertexShader(_instance->GetDevice());
    auto fragmentShader = Utilities::GetStandardFragmentShader(_instance->GetDevice());
    auto rayGen = Utilities::GetStandardRayGenShader(_instance->GetDevice());
    //TODO: Change name to GetStandardRayClosestHitShader
    auto rchit = Utilities::GetStandardRayHitShader(_instance->GetDevice());
    auto rmiss = Utilities::GetStandardRayMissShader(_instance->GetDevice());

    _pipeline = new GraphicsPipeline(vertexShader,
                                     fragmentShader,
                                     _swapChain->GetSwapChainExtent(),
                                     _descriptorManager,
                                     _instance,
                                     _renderPass);

    if (RTXon)
    {
        InitRT();
        _descriptorManager->CreateDescriptorSetLayoutRT((uint32_t)_meshes.size(), (uint32_t)_meshInstances.size());
        _pipelineRT = new GraphicsPipeline(rayGen,
                                           rchit,
                                           rmiss,
                                           _swapChain->GetSwapChainExtent(),
                                           _descriptorManager,
                                           _instance,
                                           _renderPass);

        _accelerationStructure = new AccelerationStructure(_instance, _deviceMemoryManager,
                                                           _commandBufferManager, _meshes, _meshInstances);

        CreateShaderBindingTable();
        _offScreenImageRT = _deviceMemoryManager->CreateImage(_initInfo.Width, _initInfo.Height, _swapChain->GetSwapChainImageFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        _offScreenImageView = _imageManager->CreateImageView(_offScreenImageRT, _swapChain->GetSwapChainImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT);

        //instance buffer maps an instance to the mesh
        _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, MemProps::HOST, sizeof(uint32_t) * _meshInstances.size(), _instanceBuffer);
        _deviceMemoryManager->ModifyBufferData<uint32_t>(_instanceBuffer, [&](uint32_t *data) {
            for (uint32_t instanceIndex = 0; instanceIndex < _meshInstances.size(); instanceIndex++)
            {
                data[instanceIndex] = _meshInstances[instanceIndex].mesh;
            }
        });

        _descriptorManager->CreateDescriptorSetRT(_accelerationStructure, _offScreenImageView, _globalUniformBuffer, _meshes, _meshInstances, _instanceBuffer);
        //RecordCommandBuffersRT();
    }

    _descriptorManager->CreateDescriptorPool(_swapChain, _meshInstances);
    _descriptorManager->CreateDescriptorSets(_meshInstances, _textures, _globalUniformBuffer);
    UploadGlobalUniform();
    if (RTXon)
    {
        RecordCommandBuffersRT();
        RecordRenderPass();
    }
    else
    {
        RecordRenderPass();
    }

    CreateSyncObjects();
}

void Renderer::SetRenderMode(RenderMode mode)
{
    _renderMode = mode;
}

void Renderer::ClearAllMeshData()
{
}

void Renderer::Draw(RenderPassInfo rpInfo)
{
}

void Renderer::Draw()
{
    vkWaitForFences(_instance->GetDevice(), 1, &_inFlightFences[_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    vkResetFences(_instance->GetDevice(), 1, &_inFlightFences[_currentFrame]);

    float time = std::chrono::duration_cast<FpSeconds>(Clock::now() - _lastFrameEnd).count();
    while (time < _minFrameTime)
    {
        time = std::chrono::duration_cast<FpSeconds>(Clock::now() - _lastFrameEnd).count();
    }

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_instance->GetDevice(), _swapChain->GetSwapChain(), std::numeric_limits<uint64_t>::max(), _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    VkCommandBuffer cmdBuffer;
    if (_renderMode == RenderMode::RASTERIZE)
    {
        cmdBuffer = _commandBufferManager->GetCommandBuffer(imageIndex);
    }
    else
    {
        cmdBuffer = _commandBufferManager->GetCommandBufferRT(imageIndex);
    }
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(_instance->GetDevice(), 1, &_inFlightFences[_currentFrame]);

    VkResult code = vkQueueSubmit(_instance->GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]);
    Utilities::CheckVkResult(code, "Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain->GetSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(_instance->GetPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _frameChanged)
    {
        _frameChanged = false;
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
    _lastFrameEnd = Clock::now();
    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::MarkDirty(MeshHandle mesh)
{
}

void Renderer::SetInstanceMaterial(MeshInstanceHandle instance, Material &mat)
{
     _deviceMemoryManager->ModifyBufferData<MeshUniformData>(_meshInstances[instance].uniformBuffer, [&](MeshUniformData *data) {
        data->Ambient = mat.Ambient;
        data->Diffuse = mat.Diffuse;
        data->Specular = mat.Specular;
        data->Shininess = mat.Shininess;
        data->Reflectivity = mat.Reflectivity;
        data->Transparency = mat.Transparency;
        //data->HasTexture = mat.Texture != null;
    });
}

void Renderer::SetMeshTransform(MeshInstanceHandle mesh, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
{
    glm::mat4 rotation = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);
    glm::mat4 translation = glm::translate(pos);
    glm::mat4 scale = glm::scale(scl);
    auto modelMatrix = translation * rotation * scale;
    _deviceMemoryManager->ModifyBufferData<MeshUniformData>(_meshInstances[mesh].uniformBuffer, [&](MeshUniformData *data) {
        data->ModelMatrix = modelMatrix;
    });

    //Don't update the structure if it does not exist (2 cases: no ray tracing, or renderer not finalized)
    if (_accelerationStructure != nullptr)
    {
        _accelerationStructure->UpdateInstanceTransform(mesh, modelMatrix);
    }
}

DirectionalLightHandle Renderer::AddDirectionalLight(DirectionalLight light)
{
    _directionalLights.push_back(light);
    return (DirectionalLightHandle)_directionalLights.size() - 1;
}

PointLightHandle Renderer::AddPointLight(PointLight light)
{
    _pointLights.push_back(light);
    return (PointLightHandle)_pointLights.size() - 1;
}

void Renderer::SetDirectionalLightProperties(DirectionalLightHandle light, std::function<void(DirectionalLight &)> mutator)
{
    mutator(_directionalLights[light]);
    _deviceMemoryManager->ModifyBufferData<GlobalUniformData>(_globalUniformBuffer, [mutator, light](GlobalUniformData *data) {
        mutator(data->DirectionalLights[light]);
    });
}

void Renderer::SetPointLightProperties(PointLightHandle light, std::function<void(PointLight &)> mutator)
{
    mutator(_pointLights[light]);
    _deviceMemoryManager->ModifyBufferData<GlobalUniformData>(_globalUniformBuffer, [mutator, light](GlobalUniformData *data) {
        mutator(data->PointLights[light]);
    });
}

void Renderer::SetClearColor(glm::vec4 color)
{
    _globalUniform.ClearColor = color;
}

void Renderer::SetCamera(Camera camera)
{
    _globalUniform.Position = glm::vec4(camera.Position, 1.0f);
    _globalUniform.ViewMatrix = camera.ViewMatrix;
    _globalUniform.ProjectionMatrix = camera.ProjectionMatrix;
    UploadGlobalUniform();
}

ShaderHandle Renderer::UploadShader(Shader shader)
{
    ShaderInfo info = {};
    info.Type = shader.Type;
    info.Module = Utilities::CreateShaderModule(Utilities::ReadEngineAsset(shader.FilePath), _instance->GetDevice());
    _shaders.push_back(info);
    return (ShaderHandle)_shaders.size() - 1;
}

const std::vector<const char *> validationLayers =
    {
        "VK_LAYER_LUNARG_standard_validation"};

VkDebugUtilsMessengerEXT callback;

bool CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void CreateInstance(
    std::string appName,
    VkInstance *instance,
    std::vector<const char *> extensions,
    bool enableValidationLayers)
{
    if (enableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pCallback)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void SetupDebugCallback(VkInstance instance)
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

} // namespace RTE::Rendering