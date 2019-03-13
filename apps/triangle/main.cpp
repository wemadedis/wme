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

#include "Utilities.h"
#include "DeviceMemoryManager.h"
#include "GraphicsPipeline.hpp"
#include "RenderPass.hpp"
#include "Instance.hpp"
#include "SwapChain.hpp"
#include "CommandBufferManager.hpp"
#include "ImageManager.hpp"
#include "Mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};


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
		initWindow();
		//Mesh* mesh = MakeCylinder(1.0f, 2.0f, 32);
		//mesh->pos = {0.0f, 0.0f, 1.0f};
		//meshes.push_back(mesh);
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

	VkInstance instance;

	VkDebugUtilsMessengerEXT callback;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	
	VkDevice device;
	VkQueue graphicsQueue;

	VkSurfaceKHR surface;
	VkQueue presentQueue;


	//Swap chain
	VkExtent2D swapChainExtent;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkDescriptorSetLayout descriptorSetLayout;


	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	bool framebufferResized = false;


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
	
	Image depthImage = {};

	void initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		auto app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	void initVulkan()
	{
		rendererInstance = new Instance(getRequiredExtensions(), [this](VkSurfaceKHR &surface, VkInstance instance) {
			if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create window surface!");
			}
		});

		instance = rendererInstance->GetInstance();
		surface = rendererInstance->GetSurface();
		physicalDevice = rendererInstance->GetPhysicalDevice();
		device = rendererInstance->GetDevice();
		graphicsQueue = rendererInstance->GetGraphicsQueue();
		presentQueue = rendererInstance->GetPresentQueue();

		
		

		createSwapChain();
		renderPass = new RenderPass(rendererInstance, swpchain);
		
		createDescriptorSetLayout();
		createGraphicsPipeline();

		cmdbManager = new CommandBufferManager(rendererInstance, (uint32_t)swpchain->GetSwapChainImages().size());
		imageManager = new ImageManager(rendererInstance, cmdbManager);

		DeviceMemoryManager::Initialize(rendererInstance, cmdbManager);
		depthImage = imageManager->CreateDepthImage(swapChainExtent.width, swapChainExtent.height);
		createFramebuffers();
		for(unsigned int meshIndex = 0; meshIndex < meshes.size(); meshIndex++){
			createVertexBuffer(meshes[meshIndex]);
			createIndexBuffer(meshes[meshIndex]);
			if(meshIndex == 0) CreateTextureImage(meshes[meshIndex], "textures/aa_beauty_and_the_sun.png");
			else CreateTextureImage(meshes[meshIndex], "textures/autismus.png");
		}
		
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();
		createSyncObjects();
	}

	
	void createSwapChain()
	{
		swpchain = new SwapChain(rendererInstance, WIDTH, HEIGHT);
		swapChainExtent = swpchain->GetSwapChainExtent();
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

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = cmdbManager->BeginCommandBufferInstance();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		//Old layout can be set to VK_IMAGE_LAYOUT_UNDEFINED if the content of the image is unimportant
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
		//Next two are used if transferring queue family ownership. Otherwise must be set to VK_QUEUE_FAMILY_IGNORED
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//image and subresourceRange specify the affected image and its specific parts.
        barrier.image = image;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (Utilities::HasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
		//No mipmap levels
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
		//For synchronization purposes: specifi which types of operations involvling the resource happens
		//before the barrier, and which operations must wait on the barrier.
        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }
		//
        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0, //Either 0 or VK_DEPENDENCY_BY_REGION_BIT allowing to read data if that part is written to
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
		cmdbManager->SubmitCommandBufferInstance(commandBuffer, graphicsQueue);
    }


    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, ImageInformation &info) {
		info = DeviceMemoryManager::CreateImage(width, height, format, tiling, usage);
    }

	void createDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(meshes.size(), descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(meshes.size());
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(meshes.size());
		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		
		for (size_t i = 0; i < meshes.size(); i++) {
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = meshes[i]->uniformBuffer.buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = meshes[i]->texture.image.imageView;
			imageInfo.sampler = meshes[i]->texture.sampler;
			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void createDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(swpchain->GetSwapChainImageCount());
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(swpchain->GetSwapChainImageCount());

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(swpchain->GetSwapChainImageCount());

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void createUniformBuffers() {
		size_t bufferSize = sizeof(UniformBufferObject);

		for (size_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++) {
			BufferInformation info = {}; //<-----------------------------------------------------
			DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, DeviceMemoryManager::MemProps::HOST, bufferSize, meshes[meshIndex]->uniformBuffer);
		}
	}


	void createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
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
		for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
		{
			vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
		}

		cmdbManager->DeallocateCommandBuffers();

		delete pipeline;
		
		delete renderPass;

		delete swpchain;
	}

	void recreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0)
		{

			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);

		cleanupSwapChain();

		createSwapChain();
		renderPass = new RenderPass(rendererInstance, swpchain);
		createGraphicsPipeline();
		depthImage = imageManager->CreateDepthImage(swapChainExtent.width, swapChainExtent.height);
		createFramebuffers();
		createCommandBuffers();
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

	void createCommandBuffers()
	{
		//cmdbManager = new CommandBufferManager(rendererInstance, swapChainFramebuffers.size());

		uint32_t cmdBufCount = cmdbManager->GetCommandBufferCount();

		for (uint32_t bufferIndex = 0; bufferIndex < cmdBufCount; bufferIndex++)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			VkCommandBuffer cmdBuffer = cmdbManager->GetCommandBuffer(bufferIndex);
			if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass->GetHandle();
			renderPassInfo.framebuffer = swapChainFramebuffers[bufferIndex];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = swapChainExtent;

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
			clearValues[1].depthStencil = {1.0f, 0};

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			
			vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetHandle());
			
			for(unsigned int meshIndex = 0; meshIndex < meshes.size(); meshIndex++){
				Mesh* mesh = meshes[meshIndex];
				VkBuffer vertexBuffers[] = { mesh->vertexBuffer.buffer };
				VkDeviceSize offsets[] = { 0 };
				
				vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
				
				vkCmdBindIndexBuffer(cmdBuffer, mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(), 0, 1, &descriptorSets[meshIndex], 0, nullptr);

				vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
			}
			vkCmdEndRenderPass(cmdBuffer);
			if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void createFramebuffers()
	{
		uint32_t frameBufferCount = swpchain->GetSwapChainImageCount();
		std::vector<Image> &swapChainImages = swpchain->GetSwapChainImages();
		swapChainFramebuffers.resize(frameBufferCount);
		for (size_t i = 0; i < frameBufferCount; i++)
		{
			std::array<VkImageView, 2> attachments = {
				swapChainImages[i].imageView,
				depthImage.imageView
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass->GetHandle();
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void createGraphicsPipeline()
	{
		std::ostringstream ss;
		ss << ENGINE_ASSET_DIR << "shaders/vert.spv";

		auto vertShaderCode = Utilities::ReadFile(ss.str().c_str());
		ss.str("");
		ss.clear();
		ss << ENGINE_ASSET_DIR << "shaders/frag.spv";
		std::cout << ss.str() << std::endl;
		auto fragShaderCode = Utilities::ReadFile(ss.str().c_str());
		ss.str("");
		ss.clear();
		VkShaderModule vertShaderModule = Utilities::CreateShaderModule(vertShaderCode, rendererInstance->GetDevice());
		VkShaderModule fragShaderModule = Utilities::CreateShaderModule(fragShaderCode, rendererInstance->GetDevice());

		pipeline = new GraphicsPipeline(vertShaderModule, fragShaderModule, swapChainExtent, descriptorSetLayout, device, renderPass);
		//After creating the pipeline the modules can be deleted
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);

	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
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

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
		{
			framebufferResized = false;
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
		ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
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
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	std::vector<const char *> getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
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