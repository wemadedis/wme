#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>


#include "rte/Renderer.h"
#include "RenderLogicStructs.h"

#include "Instance.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace RTE::Renderer
{

const int MAX_FRAMES_IN_FLIGHT = 2;

void Renderer::Initialize()
{
    _instance = new Instance(_initInfo.extensions, _initInfo.BindingFunc);
    _swapChain = new SwapChain(_instance, _initInfo.Widht, _initInfo.Height);
    _renderPass = new RenderPass(_instance, _swapChain);
    _descriptorManager = new DescriptorManager(_instance);
    _descriptorManager->CreateDescriptorSetLayout();
    _pipeline = new GraphicsPipeline(   "shaders/vert.spv", 
                                        "shaders/frag.spv", 
                                        _swapChain->GetSwapChainExtent(), 
                                        _descriptorManager->GetDescriptorLayout(), 
                                        _instance->GetDevice(), 
                                        _renderPass);
    _commandBufferManager = new CommandBufferManager(_instance, (uint32_t)_swapChain->GetSwapChainImageCount());
    _imageManager = new ImageManager(_instance, _commandBufferManager);
    DeviceMemoryManager::Initialize(_instance, _commandBufferManager);
    _swapChain->CreateFramebuffers(_renderPass, _imageManager);

}

void Renderer::CreateTexture(MeshInfo* mesh, const char *imagePath){
    int texWidth, texHeight, texChannels;
    std::ostringstream ss;
    ss << ENGINE_ASSET_DIR << imagePath;

    stbi_uc* pixels = stbi_load(ss.str().c_str() , &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    uint32_t imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    mesh->texture = _imageManager->CreateTexture(texWidth, texHeight, pixels, imageSize);
}

MeshHandle Renderer::UploadMesh(Mesh* mesh)
{
    MeshInfo* info = new MeshInfo();
    info->IndexCount = mesh->indices.size();
    size_t bufferSize = sizeof(mesh->indices[0]) * info->IndexCount;
    
    //Indices
    DeviceMemoryManager::BufferInformation stagingBuffer = {};
    DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, DeviceMemoryManager::MemProps::HOST, bufferSize, stagingBuffer);
    DeviceMemoryManager::CopyDataToBuffer(stagingBuffer, (void*)mesh->indices.data());
    DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, DeviceMemoryManager::MemProps::DEVICE, bufferSize, info->indexBuffer);
    DeviceMemoryManager::CopyBuffer(stagingBuffer, info->indexBuffer, bufferSize, _commandBufferManager->GetCommandPool(), _instance->GetGraphicsQueue());
    DeviceMemoryManager::DestroyBuffer(stagingBuffer);
    //Vertices
    bufferSize = sizeof(mesh->vertices[0]) * mesh->vertices.size();
    DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, DeviceMemoryManager::MemProps::HOST, bufferSize, stagingBuffer);
    DeviceMemoryManager::CopyDataToBuffer(stagingBuffer, (void*)mesh->vertices.data());
    DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, DeviceMemoryManager::MemProps::DEVICE, bufferSize, info->vertexBuffer);
    DeviceMemoryManager::CopyBuffer(stagingBuffer, info->vertexBuffer, bufferSize, _commandBufferManager->GetCommandPool(), _instance->GetGraphicsQueue());
    //Possibly destroy vertices in host memory
    DeviceMemoryManager::DestroyBuffer(stagingBuffer);
    DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, DeviceMemoryManager::MemProps::HOST, sizeof(UniformBufferObject), info->uniformBuffer);
    
    UniformBufferObject ubo = {};
    glm::mat4 rot = glm::eulerAngleXYZ(0, 0, 0);
    glm::mat4 trn = glm::translate(glm::vec3(0.0f, -0.5f, 1.0f));
    glm::mat4 scl = glm::scale(glm::vec3(1.0f));
    ubo.model = trn * rot  * scl;
    ubo.view = glm::lookAt(glm::vec3(0.0f, 2.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), _swapChain->GetSwapChainExtent().width / (float)_swapChain->GetSwapChainExtent().height, 0.1f, 100.0f);
    ubo.proj[1][1] *= -1;

    DeviceMemoryManager::CopyDataToBuffer(info->uniformBuffer, &ubo);
    
    CreateTexture(info, "textures/aa_beauty_and_the_sun.png");
    
    _meshes.push_back(info);
    return _meshes.size()-1;
}

TextureHandle Renderer::UploadTexture(Texture &texture)
{
    _textures.push_back(_imageManager->CreateTexture(texture.Width, texture.Height, texture.Pixels, texture.Width*texture.Height*4));
    return _textures.size()-1;
}

void Renderer::BindTexture(TextureHandle texture, MeshHandle mesh)
{
    _meshes[mesh]->texture = _textures[texture];
}

void Renderer::RecordRenderPass()
{
    for (uint32_t bufferIndex = 0; bufferIndex < _commandBufferManager->GetCommandBufferCount(); bufferIndex++)
    {
        VkCommandBuffer cmdBuffer = _commandBufferManager->GetCommandBuffer(bufferIndex);
        _renderPass->BeginRenderPass(_pipeline, cmdBuffer, _swapChain->GetFramebuffers()[bufferIndex]);

        for(unsigned int meshIndex = 0; meshIndex < _meshes.size(); meshIndex++){
            MeshInfo* mesh = _meshes[meshIndex];
            VkBuffer vertexBuffers[] = { mesh->vertexBuffer.buffer };
            VkDeviceSize offsets[] = { 0 };
            
            vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
            
            vkCmdBindIndexBuffer(cmdBuffer, mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->GetLayout(), 0, 1, &_descriptorManager->GetDescriptorSets()[meshIndex], 0, nullptr);

            vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(mesh->IndexCount), 1, 0, 0, 0);
        }

        _renderPass->EndRenderPass(cmdBuffer);
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

    delete _pipeline;
    
    delete _renderPass;

    delete _swapChain;
}

void Renderer::RecreateSwapChain()
{
    int width = _initInfo.Widht;   //win->Width;
    int height = _initInfo.Height; //win->Height;

    vkDeviceWaitIdle(_instance->GetDevice());

    CleanupSwapChain();
    _commandBufferManager->AllocateCommandBuffers();
    _swapChain = new SwapChain(_instance, width, height);
    _renderPass = new RenderPass(_instance, _swapChain);
    _pipeline = new GraphicsPipeline(   "shaders/vert.spv", 
                                        "shaders/frag.spv", 
                                        _swapChain->GetSwapChainExtent(), 
                                        _descriptorManager->GetDescriptorLayout()
                                        , _instance->GetDevice(), _renderPass);
    _swapChain->CreateFramebuffers(_renderPass, _imageManager);
    RecordRenderPass();
}

Renderer::Renderer(RendererInitInfo info)
{
    _initInfo = info;
    Initialize();
}

void Renderer::Finalize()
{
    _descriptorManager->CreateDescriptorPool(_swapChain);
    _descriptorManager->CreateDescriptorSets(_meshes);
    RecordRenderPass();
    CreateSyncObjects();
}

void Renderer::SetRenderMode(RenderMode mode)
{

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

    for(unsigned int meshIndex = 0; meshIndex < _meshes.size(); meshIndex++){
        //updateUniformBuffer(meshes[meshIndex]);
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    VkCommandBuffer cmbBuffer = _commandBufferManager->GetCommandBuffer(imageIndex);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmbBuffer;

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(_instance->GetDevice(), 1, &_inFlightFences[_currentFrame]);

    if (vkQueueSubmit(_instance->GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

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

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)// || win->WindowResized)
    {
        //win->WindowResized = false;
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::MarkDirty(MeshHandle mesh)
{
    
}

void Renderer::AddLight(Light light)
{
    
}

void SetCamera(Camera camera)
{
    
}

void Renderer::UploadShader(Shader shader)
{
    
}


const std::vector<const char *> validationLayers = 
{
    "VK_LAYER_LUNARG_standard_validation"
};

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

} // namespace RTE::Renderer