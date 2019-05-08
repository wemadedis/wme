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

bool Renderer::IsRaytracingCapable()
{
    return Instance::IsRayTracingCapable();
}

void Renderer::FrameResized(int32_t width, int32_t height)
{
    _frameWidth = width;
    _frameHeight = height;
    _frameChanged = true;
}

glm::ivec2 Renderer::GetFrameSize()
{
    return {_frameWidth, _frameHeight};
}

void Renderer::Initialize()
{
    _instance = new Instance(_initInfo.extensions, _initInfo.BindingFunc, _initInfo.RayTracingOn);
    _rtxOn = _initInfo.RayTracingOn;

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
    MeshInfo info ;
    info.IndexCount = mesh.Indices.size();
    info.VertexCount = mesh.Vertices.size();
    size_t bufferSize = (size_t)(sizeof(mesh.Indices[0]) * info.IndexCount);

    if (bufferSize == 0)
    {
        throw std::invalid_argument("Renderer: Trying to create buffer with size 0");
    }

    //Indices
    BufferInformation stagingBuffer = {};
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemProps::HOST, bufferSize, stagingBuffer);
    _deviceMemoryManager->CopyDataToBuffer(stagingBuffer, (void *)mesh.Indices.data());
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, MemProps::DEVICE, bufferSize, info.indexBuffer);
    _deviceMemoryManager->CopyBuffer(stagingBuffer, info.indexBuffer, bufferSize, _commandBufferManager->GetCommandPool(), _instance->GetGraphicsQueue());
    _deviceMemoryManager->DestroyBuffer(stagingBuffer);
    //Vertices
    bufferSize = sizeof(mesh.Vertices[0]) * mesh.Vertices.size();
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemProps::HOST, bufferSize, stagingBuffer);
    _deviceMemoryManager->CopyDataToBuffer(stagingBuffer, (void *)mesh.Vertices.data());
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, MemProps::DEVICE, bufferSize, info.vertexBuffer);
    _deviceMemoryManager->CopyBuffer(stagingBuffer, info.vertexBuffer, bufferSize, _commandBufferManager->GetCommandPool(), _instance->GetGraphicsQueue());

    _deviceMemoryManager->DestroyBuffer(stagingBuffer);

    _meshes.push_back(info);
    return (MeshHandle)_meshes.size() - 1;
}

MeshInstanceHandle Renderer::CreateMeshInstance(MeshHandle mesh)
{
    MeshInstance instance = {};
    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, MemProps::HOST, sizeof(MeshInstanceUniformData), instance.uniformBuffer);

    MeshInstanceUniformData meshUniform = {};
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
    _deviceMemoryManager->ModifyBufferData<MeshInstanceUniformData>(_meshInstances[instance].uniformBuffer, [&](MeshInstanceUniformData *data) {
        data->Texture = texture;
        //If set to empty texture do not use texture mapping, otherwise do.
        data->HasTexture = texture != Renderer::EMPTY_TEXTURE;
    });
}

void Renderer::RecordRenderPass()
{
    for (uint32_t bufferIndex = 0; bufferIndex < _commandBufferManager->GetCommandBufferCount(); bufferIndex++)
    {
        VkCommandBuffer cmdBuffer = _commandBufferManager->GetCommandBuffer(bufferIndex);
        _renderPass->BeginRenderPass(cmdBuffer, _swapChain->GetFramebuffers()[bufferIndex], _globalUniform.ClearColor);

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->GetHandle());

        for (unsigned int meshIndex = 0; meshIndex < _meshInstances.size(); meshIndex++)
        {
            MeshInfo *mesh = &_meshes[_meshInstances[meshIndex].mesh];
            VkBuffer vertexBuffers[] = {mesh->vertexBuffer.buffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(cmdBuffer, mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->GetLayout(), 0, 1, &_descriptorManager->GetDescriptorSets()[meshIndex], 0, nullptr);

            vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(mesh->IndexCount), 1, 0, 0, 0);
        }
        
        if(_lineModule != nullptr)
        {
            auto lines = _lineModule->GetLineData();
            if(_lineBuffer.buffer == VK_NULL_HANDLE)
            {
                _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, MemProps::HOST , sizeof(Line)*1000, _lineBuffer);
            }
            //Requires at least one mesh instance in order to draw the lines, as the drawing needs a descriptor set for the view & projection matrices.
            if(lines.size() > 0 && _meshInstances.size() > 0)
            {
                _deviceMemoryManager->CopyDataToBuffer(_lineBuffer, lines.data(), sizeof(Line)*lines.size());
                
                vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _linePipeline->GetHandle());
                VkDeviceSize offsets[] = {0};
                vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _linePipeline->GetLayout(), 0, 1, &_descriptorManager->GetDescriptorSets()[0], 0, nullptr);
                vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &_lineBuffer.buffer, offsets);
                vkCmdDraw(cmdBuffer, (uint32_t)lines.size()*2, 1, 0, 0);
                _lineModule->ClearData();
            }
        }
        _renderPass->NextSubpass(cmdBuffer, VK_SUBPASS_CONTENTS_INLINE);
        if (_guiModule != nullptr)
        {
            _guiModule->Draw(cmdBuffer, _frameWidth, _frameHeight);
        }
        _renderPass->EndRenderPass(cmdBuffer);
    }
}

void Renderer::RecordRenderPassRT()
{
    auto commandBuffer = _commandBufferManager->BeginCommandBufferInstance();
    _accelerationStructure->RebuildTopStructureCmd(commandBuffer);
    _commandBufferManager->SubmitCommandBufferInstance(commandBuffer, _instance->GetGraphicsQueue());

    for (uint32_t bufferIndex = 0; bufferIndex < _commandBufferManager->GetCommandBufferCount(); bufferIndex++)
    {
        const VkCommandBuffer commandBuffer = _commandBufferManager->GetCommandBufferRT(bufferIndex);
        _renderPass->BeginRenderPass(commandBuffer, _swapChain->GetFramebuffers()[bufferIndex], _globalUniform.ClearColor);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, _pipelineRT->GetHandle());
        //Buffer index = frame index
        _descriptorManager->UpdateRTTargetImage(_swapChain->GetSwapChainImages()[bufferIndex].imageView);
        auto dset = _descriptorManager->GetDescriptorSetRT();
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, _pipelineRT->GetLayout(), 0, (uint32_t)dset.size(), dset.data(), 0, 0);
        auto extent = _swapChain->GetSwapChainExtent();
        RTUtilities::GetInstance()->vkCmdTraceRaysNV(commandBuffer,
                                                     _shaderBindingTable.buffer, 0,
                                                     _shaderBindingTable.buffer, 3 * _rtProperties.shaderGroupHandleSize, _rtProperties.shaderGroupHandleSize,
                                                     _shaderBindingTable.buffer, 1 * _rtProperties.shaderGroupHandleSize, _rtProperties.shaderGroupHandleSize,
                                                     VK_NULL_HANDLE, 0, 0,
                                                     extent.width, extent.height, 1);

        _renderPass->NextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
        if (_guiModule != nullptr)
        {
            _guiModule->Draw(commandBuffer, _frameWidth, _frameHeight);
        }
        _renderPass->EndRenderPass(commandBuffer);
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
    if (_rtxOn)
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
    if (_rtxOn)
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
                                     _renderPass, 
                                     VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
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

void Renderer::GetRTProperties()
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
    const uint32_t groupNum = 5;
    const uint32_t shaderBindingTableSize = _rtProperties.shaderGroupHandleSize * groupNum;

    _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemProps::HOST, shaderBindingTableSize, _shaderBindingTable);

    _deviceMemoryManager->ModifyBufferData<void *>(_shaderBindingTable, [&](void *data) {
        VkResult code = RTUtilities::GetInstance()->vkGetRayTracingShaderGroupHandlesNV(_instance->GetDevice(), _pipelineRT->GetHandle(), 0, groupNum, shaderBindingTableSize, data);
        Utilities::CheckVkResult(code, "Failed to get RT shader group handles!");
    });
}

Renderer::Renderer(RendererInitInfo info)
{
    _initInfo = info;
    _frameWidth = info.Width;
    _frameHeight = info.Height;
    Initialize();
    if (_rtxOn)
    {
        _renderMode = RenderMode::RAYTRACE;
    }
    else
    {
        _renderMode = RenderMode::RASTERIZE;
    }
}

Renderer::Renderer(RendererInitInfo info, GUI::GUIModule *guiModule) : Renderer(info)
{
    _guiModule = guiModule;
}

void Renderer::Finalize()
{

    _descriptorManager = new DescriptorManager(_instance);
    _descriptorManager->CreateDescriptorSetLayout();

    auto vertexShader = Utilities::GetStandardVertexShader(_instance->GetDevice());
    auto fragmentShader = Utilities::GetStandardFragmentShader(_instance->GetDevice());

    _pipeline = new GraphicsPipeline(vertexShader,
                                     fragmentShader,
                                     _swapChain->GetSwapChainExtent(),
                                     _descriptorManager,
                                     _instance,
                                     _renderPass,
                                     VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    if(_lineModule != nullptr)
    {
        auto vertexShader = Utilities::GetStandardLineVertexShader(_instance->GetDevice());
        auto fragmentShader = Utilities::GetStandardLineFragmentShader(_instance->GetDevice());
        _linePipeline = new GraphicsPipeline(   vertexShader,
                                                fragmentShader,
                                                _swapChain->GetSwapChainExtent(),
                                                _descriptorManager,
                                                _instance,
                                                _renderPass,
                                                VK_PRIMITIVE_TOPOLOGY_LINE_LIST); 
    }
    if (_rtxOn)
    {
        auto rayGen = Utilities::GetStandardRayGenShader(_instance->GetDevice());
        //TODO: Change name to GetStandardRayClosestHitShader
        auto rchit = Utilities::GetStandardRayHitShader(_instance->GetDevice());
        auto rmiss = Utilities::GetStandardRayMissShader(_instance->GetDevice());
        auto srchit = Utilities::GetShadowRayHitShader(_instance->GetDevice());
        auto srmiss = Utilities::GetShadowdRayMissShader(_instance->GetDevice());

        GetRTProperties();
        _descriptorManager->CreateDescriptorSetLayoutRT((uint32_t)_meshes.size(), (uint32_t)_meshInstances.size(), (uint32_t)_textures.size());
        _pipelineRT = new GraphicsPipeline(rayGen,
                                           rchit,
                                           rmiss,
                                           srchit,
                                           srmiss,
                                           _swapChain->GetSwapChainExtent(),
                                           _descriptorManager,
                                           _instance,
                                           _renderPass);

        _accelerationStructure = new AccelerationStructure(_instance, _deviceMemoryManager,
                                                           _commandBufferManager, _meshes, _meshInstances);

        CreateShaderBindingTable();

        //instance buffer maps an instance to the mesh
        if(_meshInstances.size() == 0)
        {
            //If no instances exist, create a buffer 
            _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, MemProps::HOST, sizeof(uint32_t), _instanceBuffer);
        }
        else
        {
            _deviceMemoryManager->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, MemProps::HOST, sizeof(uint32_t) * _meshInstances.size(), _instanceBuffer);
        }
        
        _deviceMemoryManager->ModifyBufferData<uint32_t>(_instanceBuffer, [&](uint32_t *data) {
            for (uint32_t instanceIndex = 0; instanceIndex < _meshInstances.size(); instanceIndex++)
            {
                data[instanceIndex] = _meshInstances[instanceIndex].mesh;
            }
        });

        _descriptorManager->CreateDescriptorSetRT(_accelerationStructure, _swapChain->GetSwapChainImages()[_currentFrame].imageView, _globalUniformBuffer, _meshes, _meshInstances, _instanceBuffer, _textures);
    }

    _descriptorManager->CreateDescriptorPools(_swapChain, _meshInstances);
    _descriptorManager->CreateDescriptorSets(_meshInstances, _textures, _globalUniformBuffer);
    UploadGlobalUniform();

    

    if (_guiModule != nullptr)
    {
        GUI::GUIInitInfo guiInfo;
        guiInfo.Instance = _instance->GetInstance();
        guiInfo.PhysicalDevice = _instance->GetPhysicalDevice();
        guiInfo.Device = _instance->GetDevice();
        guiInfo.QueueFamily = VK_QUEUE_FAMILY_IGNORED;
        guiInfo.Queue = _instance->GetGraphicsQueue();
        guiInfo.PipelineCache = VK_NULL_HANDLE;
        guiInfo.DescriptorPool = _descriptorManager->GetDescriptorPool();
        guiInfo.MinImageCount = _swapChain->GetSwapChainImageCount();
        guiInfo.ImageCount = guiInfo.MinImageCount;
        guiInfo.Allocator = nullptr;
        guiInfo.CheckVkResultFn = [](VkResult code) { Utilities::CheckVkResult(code, "ImGUI Error"); };

        VkCommandBuffer cmdBuffer = _commandBufferManager->BeginCommandBufferInstance();

        _guiModule->Initialize(guiInfo, _renderPass->GetHandle(), cmdBuffer);
        
        _commandBufferManager->SubmitCommandBufferInstance(cmdBuffer, _instance->GetGraphicsQueue());
    }


    if (_rtxOn)
    {
        RecordRenderPassRT();
    }
    RecordRenderPass();

    CreateSyncObjects();
}

void Renderer::SetRenderMode(RenderMode mode)
{
    //If trying to set to ray tracing and it is not available, do not change to render mode.
    if(mode == RenderMode::RAYTRACE && !_rtxOn) return;
    _renderMode = mode;
}

void Renderer::Draw()
{
    vkWaitForFences(_instance->GetDevice(), 1, &_inFlightFences[_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    vkResetFences(_instance->GetDevice(), 1, &_inFlightFences[_currentFrame]);

    if (_renderMode == RenderMode::RASTERIZE)
    {
        RecordRenderPass();
    }
    else
    {
        RecordRenderPassRT();
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

    VkCommandBuffer cmdBuffer;
    if (_renderMode == RenderMode::RASTERIZE)
    {
        cmdBuffer = _commandBufferManager->GetCommandBuffer(imageIndex);
    }
    else
    {
        cmdBuffer = _commandBufferManager->GetCommandBufferRT(imageIndex);
    }

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; //TODO: ANALYZE THIS: WAS AT THE END OF THIS FUNC BEFORE AND INTRODUCED INCONSISTENCY IN SUBMITTING & RECORDING COMMAND BUFFERS

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

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
}

void Renderer::SetInstanceMaterial(MeshInstanceHandle instance, Material &mat)
{
    _deviceMemoryManager->ModifyBufferData<MeshInstanceUniformData>(_meshInstances[instance].uniformBuffer, [&](MeshInstanceUniformData *data) {
        data->Ambient = mat.Ambient;
        data->Diffuse = mat.Diffuse;
        data->Specular = mat.Specular;
        data->Shininess = mat.Shininess;
        data->Reflectivity = mat.Reflectivity;
        data->Transparency = mat.Transparency;
        data->Color = mat.Color;
    });
}

void Renderer::SetInstanceTransform(MeshInstanceHandle instance, glm::mat4 &modelMatrix)
{
    glm::mat4 normalMatrix;
    //Calculate the normal matrix
    if(_renderMode == RenderMode::RAYTRACE)
    {
        normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    }
    else 
    {
        //The normal is transformed to view space for further calculations in rasterization.
        normalMatrix = glm::transpose(glm::inverse(_globalUniform.ViewMatrix*modelMatrix));
    }
    
    _deviceMemoryManager->ModifyBufferData<MeshInstanceUniformData>(_meshInstances[instance].uniformBuffer, [&](MeshInstanceUniformData *data) {
        data->ModelMatrix = modelMatrix;
        data->NormalMatrix = normalMatrix;
    });

    //Don't update the structure if it does not exist (2 cases: no ray tracing, or renderer not finalized)
    if (_accelerationStructure != nullptr)
    {
        _accelerationStructure->UpdateInstanceTransform(instance, modelMatrix);
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
    //TODO: CLEAN THIS. Can the light in device memory be updated along with all other lights in one call instead?
    mutator(_directionalLights[light]);
    _deviceMemoryManager->ModifyBufferData<GlobalUniformData>(_globalUniformBuffer, [mutator, light](GlobalUniformData *data) {
        mutator(data->DirectionalLights[light]);
    });
}

void Renderer::SetPointLightProperties(PointLightHandle light, std::function<void(PointLight &)> mutator)
{
    //TODO: CLEAN THIS
    mutator(_pointLights[light]);
    _deviceMemoryManager->ModifyBufferData<GlobalUniformData>(_globalUniformBuffer, [mutator, light](GlobalUniformData *data) {
        mutator(data->PointLights[light]);
    });
}

void Renderer::SetClearColor(glm::vec4 color)
{
    _globalUniform.ClearColor = color;
    UploadGlobalUniform();
}

void Renderer::SetCamera(Camera camera)
{
    _globalUniform.Position = glm::vec4(camera.Position, 1.0f);
    _globalUniform.ViewMatrix = camera.ViewMatrix;
    _globalUniform.ProjectionMatrix = camera.ProjectionMatrix;
    _globalUniform.FieldOfView = glm::tan(glm::radians(camera.FieldOfView/2.0f));
    _globalUniform.NearPlane = camera.NearPlane;
    _globalUniform.FarPlane = camera.FarPlane;
    UploadGlobalUniform();
}

void Renderer::SetLineDebugDraw(LineDebugDrawModule *module)
{
    _lineModule = module;
}

} // namespace RTE::Rendering