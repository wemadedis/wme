#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RTE.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <array>
#include <sstream>

#include "rte/WindowManager.h"
#include "Utilities.h"
#include "DeviceMemoryManager.h"
#include "GraphicsPipeline.hpp"
#include "RenderPass.hpp"
#include "Instance.hpp"
#include "SwapChain.hpp"
#include "CommandBufferManager.hpp"
#include "ImageManager.hpp"
#include "Mesh.hpp"
#include "DescriptorManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace RTE::Renderer
{

const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}
};



const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0, 2, 1, 0, 0, 3, 2
};

class HelloTriangleApplication {
public:
	void run() {
		windowManager = RTE::Platform::WindowManager::GetInstance();
		win = windowManager->OpenWindow(WIDTH, HEIGHT, "Triangle");
		//initWindow();
		Mesh* mesh = new Mesh();
		mesh->indices = indices;
		mesh->vertices = vertices;
		mesh->pos = {1.0f, 0.0f, 1.0f};
		mesh->rot = {0.0f, glm::radians(180.0f), 0.0f};
		meshes.push_back(mesh);

		mesh = new Mesh();
		mesh->indices = indices;
		mesh->vertices = vertices;
		mesh->pos = {-1.0f, 0.0f, 1.0f};
		mesh->rot = {0.0f, glm::radians(180.0f), 0.0f};
		meshes.push_back(mesh);

		initVulkan();
		mainLoop();
		cleanup();
	}

  private:
	GLFWwindow *window;
	RTE::Platform::WindowManager *windowManager;
	RTE::Platform::RTEWindow *win = nullptr;
	VkInstance instance;

	VkDebugUtilsMessengerEXT callback;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	
	VkDevice device;
	VkQueue graphicsQueue;

	VkSurfaceKHR surface;
	VkQueue presentQueue;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	//OUR MEMORY MANAGEMENT
	typedef DeviceMemoryManager::BufferInformation BufferInformation;
	typedef DeviceMemoryManager::ImageInformation ImageInformation;
	
	std::vector<Mesh*> meshes;

	GraphicsPipeline *pipeline;
	RenderPass *renderPass;
	Instance *rendererInstance;
	SwapChain *swpchain;
	CommandBufferManager *cmdbManager;
	ImageManager *imageManager;
	DescriptorManager *descriptorManager;

	void initVulkan()
	{
		rendererInstance = new Instance(windowManager->GetRequiredExtensions(), [this](VkSurfaceKHR &surface, VkInstance instance) {
			windowManager->CreateSurface(instance, surface);
		});

		instance = rendererInstance->GetInstance();
		surface = rendererInstance->GetSurface();
		physicalDevice = rendererInstance->GetPhysicalDevice();
		device = rendererInstance->GetDevice();
		graphicsQueue = rendererInstance->GetGraphicsQueue();
		presentQueue = rendererInstance->GetPresentQueue();

		
		

		swpchain = new SwapChain(rendererInstance, WIDTH, HEIGHT);
		renderPass = new RenderPass(rendererInstance, swpchain);
		descriptorManager = new DescriptorManager(rendererInstance);
		descriptorSetLayout = descriptorManager->CreateDescriptorSetLayout();
		

		pipeline = new GraphicsPipeline("shaders/vert.spv", "shaders/frag.spv", swpchain->GetSwapChainExtent(), descriptorSetLayout, device, renderPass);
		cmdbManager = new CommandBufferManager(rendererInstance, (uint32_t)swpchain->GetSwapChainImages().size());
		imageManager = new ImageManager(rendererInstance, cmdbManager);

		DeviceMemoryManager::Initialize(rendererInstance, cmdbManager);
		swpchain->CreateFramebuffers(renderPass, imageManager);
		for(unsigned int meshIndex = 0; meshIndex < meshes.size(); meshIndex++){
			createVertexBuffer(meshes[meshIndex]);
			createIndexBuffer(meshes[meshIndex]);
			if(meshIndex == 0) CreateTextureImage(meshes[meshIndex], "textures/aa_beauty_and_the_sun.png");
			else CreateTextureImage(meshes[meshIndex], "textures/autismus.png");
		}
		
		createUniformBuffers();

		descriptorPool = descriptorManager->CreateDescriptorPool(swpchain);
		descriptorSets = descriptorManager->CreateDescriptorSets(meshes);
		
		RecordRenderPass();
		createSyncObjects();
	}

	void CreateTextureImage(Mesh* mesh, const char *imagePath){
		int texWidth, texHeight, texChannels;
		std::ostringstream ss;
		ss << ENGINE_ASSET_DIR << imagePath;

		stbi_uc* pixels = stbi_load(ss.str().c_str() , &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		uint32_t imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}
		mesh->texture = imageManager->CreateTexture(texWidth, texHeight, pixels, imageSize);
	}

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, ImageInformation &info) {
		info = DeviceMemoryManager::CreateImage(width, height, format, tiling, usage);
    }

	void createUniformBuffers() {
		size_t bufferSize = sizeof(UniformBufferObject);

		for (size_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++) {
			BufferInformation info = {}; //<-----------------------------------------------------
			DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, DeviceMemoryManager::MemProps::HOST, bufferSize, meshes[meshIndex]->uniformBuffer);
		}
	}

	void createIndexBuffer(Mesh* mesh) {
		size_t bufferSize = sizeof(mesh->indices[0]) * mesh->indices.size();

		BufferInformation stagingBuffer = {};
		DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, DeviceMemoryManager::MemProps::HOST, bufferSize, stagingBuffer);
		DeviceMemoryManager::CopyDataToBuffer(stagingBuffer, (void*)mesh->indices.data());
		DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, DeviceMemoryManager::MemProps::DEVICE, bufferSize, mesh->indexBuffer);
		DeviceMemoryManager::CopyBuffer(stagingBuffer, mesh->indexBuffer, bufferSize, cmdbManager->GetCommandPool(), graphicsQueue);

		//Possibly destroy indices in host memory
		DeviceMemoryManager::DestroyBuffer(stagingBuffer);
	}

	void createVertexBuffer(Mesh* mesh) {
		size_t bufferSize = sizeof(mesh->vertices[0]) * mesh->vertices.size();
		
		BufferInformation stagingBuffer = {};
		DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, DeviceMemoryManager::MemProps::HOST, bufferSize, stagingBuffer);
		DeviceMemoryManager::CopyDataToBuffer(stagingBuffer, (void*)mesh->vertices.data());
		DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, DeviceMemoryManager::MemProps::DEVICE, bufferSize, mesh->vertexBuffer);
		DeviceMemoryManager::CopyBuffer(stagingBuffer, mesh->vertexBuffer, bufferSize, cmdbManager->GetCommandPool(), graphicsQueue);
		//Possibly destroy vertices in host memory
		DeviceMemoryManager::DestroyBuffer(stagingBuffer);
	}

	void cleanupSwapChain()
	{
		/*vkDestroyImageView(device, depthImageView, nullptr);
        vkDestroyImage(device, depthImage, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);*/

		cmdbManager->DeallocateCommandBuffers();

		delete pipeline;
		
		delete renderPass;

		delete swpchain;
	}

	void recreateSwapChain()
	{
		int width = win->Width;
		int height = win->Height;

		vkDeviceWaitIdle(device);

		cleanupSwapChain();
		cmdbManager->AllocateCommandBuffers();
		swpchain = new SwapChain(rendererInstance, width, height);
		renderPass = new RenderPass(rendererInstance, swpchain);
		pipeline = new GraphicsPipeline("shaders/vert.spv", "shaders/frag.spv", swpchain->GetSwapChainExtent(), descriptorSetLayout, device, renderPass);
		swpchain->CreateFramebuffers(renderPass, imageManager);
		RecordRenderPass();
	}

	void createSyncObjects()
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	void RecordRenderPass()
	{
		for (uint32_t bufferIndex = 0; bufferIndex < cmdbManager->GetCommandBufferCount(); bufferIndex++)
		{
			VkCommandBuffer cmdBuffer = cmdbManager->GetCommandBuffer(bufferIndex);
			renderPass->BeginRenderPass(pipeline, cmdBuffer, swpchain->GetFramebuffers()[bufferIndex]);

			for(unsigned int meshIndex = 0; meshIndex < meshes.size(); meshIndex++){
				Mesh* mesh = meshes[meshIndex];
				VkBuffer vertexBuffers[] = { mesh->vertexBuffer.buffer };
				VkDeviceSize offsets[] = { 0 };
				
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
				
				vkCmdBindIndexBuffer(cmdBuffer, mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(), 0, 1, &descriptorSets[meshIndex], 0, nullptr);

				vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
			}

			renderPass->EndRenderPass(cmdBuffer);
		}
	}


	void mainLoop()
	{
		while (!RTE::Platform::ShouldClose(win))
		{
			RTE::Platform::PollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(device);
	}

	void drawFrame()
	{
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swpchain->GetSwapChain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		for(unsigned int meshIndex = 0; meshIndex < meshes.size(); meshIndex++){
			updateUniformBuffer(meshes[meshIndex]);
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		VkCommandBuffer cmbBuffer = cmdbManager->GetCommandBuffer(imageIndex);
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmbBuffer;

		VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {swpchain->GetSwapChain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || win->WindowResized)
		{
			win->WindowResized = false;
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void updateUniformBuffer(Mesh* mesh) {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo = {};
		glm::mat4 rot = glm::eulerAngleXYZ(mesh->rot.x, mesh->rot.y+time, mesh->rot.z);
		glm::mat4 trn = glm::translate(mesh->pos);
		glm::mat4 scl = glm::scale(mesh->scale);
		ubo.model = trn * rot  * scl;
		ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), swpchain->GetSwapChainExtent().width / (float)swpchain->GetSwapChainExtent().height, 0.1f, 100.0f);
		ubo.proj[1][1] *= -1;

		DeviceMemoryManager::CopyDataToBuffer(mesh->uniformBuffer, &ubo);
	}

	void cleanup()
	{
		cleanupSwapChain();

		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

		for(unsigned int meshIndex = 0; meshIndex < meshes.size(); meshIndex++){
			DeviceMemoryManager::DestroyBuffer(meshes[meshIndex]->indexBuffer);
			DeviceMemoryManager::DestroyBuffer(meshes[meshIndex]->vertexBuffer);
			DeviceMemoryManager::DestroyBuffer(meshes[meshIndex]->uniformBuffer);
			DeviceMemoryManager::DestroyImage(meshes[meshIndex]->texture.image.imageInfo);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		delete rendererInstance;
		RTE::Platform::DestroyWindow(win);
		RTE::Platform::Terminate();
	}
};

int main()
{
	HelloTriangleApplication app;

	try
	{
		app.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

};