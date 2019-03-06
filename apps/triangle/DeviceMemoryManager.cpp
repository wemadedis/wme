#include "DeviceMemoryManager.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <iostream>
#include <memory>
#include <map>

using namespace std;
namespace DeviceMemoryManager
{

VmaAllocator *alloc = nullptr;
map<VkBuffer, VmaAllocation> buffers;
map<VkImage, VmaAllocation> images;
Instance* _instance;
CommandBufferManager *_cmdbManager;

void Initialize(Instance *instance, CommandBufferManager *commandBufferManager)
{
    //FIX THIS MALLOC PLS <--------------------------------------------------------------------------------------------------------
    alloc = (VmaAllocator *)malloc(sizeof(VmaAllocator));
    VmaAllocatorCreateInfo info = {};
    _instance = instance;
    info.physicalDevice = _instance->GetPhysicalDevice();
    info.device = _instance->GetDevice();
    vmaCreateAllocator(&info, alloc);
}

void CreateBuffer(VkBufferUsageFlags bufferUsage, MemProps props, size_t size, BufferInformation& bufferInformation)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = bufferUsage;
    //bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    switch (props)
    {
    case MemProps::HOST:
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        break;
    case MemProps::DEVICE:
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        break;
    }

    VkBuffer buffer;
    VmaAllocation allocation;

    VkResult result = vmaCreateBuffer(*alloc, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("(DeviceMemoryManager) Failed to create buffer!");
    }
    buffers.insert(pair<VkBuffer, VmaAllocation>(buffer, allocation));
    
    bufferInformation.bufferUsage = bufferUsage;
    bufferInformation.memoryProperties = props;
    bufferInformation.size = size;
    bufferInformation.buffer = buffer;
}

void CopyDataToBuffer(BufferInformation& bufferInfo, void* data){
    void *mapping = malloc(bufferInfo.size); //<--------------------------------------- TRIED TO FREE IT AFTER UNMAP, GOT EXCEPTION (IS UNMAP FREEING IT IMPLICITLY??)
    VmaAllocation& allocation = buffers[bufferInfo.buffer];
    vmaMapMemory(*alloc, allocation, &mapping);
    memcpy(mapping, data, bufferInfo.size);
    vmaUnmapMemory(*alloc, allocation);
    
}

void CopyBuffer(BufferInformation& srcBuffer, BufferInformation& dstBuffer, size_t size, VkCommandPool commandPool, VkQueue submitQueue) {
		//May need to create a seperate command buffer pool for these short lived buffers
        cout << "keks butter" << endl;
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_instance->GetDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(submitQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(submitQueue);
		/*
		A fence would allow you to schedule multiple transfers simultaneously 
		and wait for all of them complete, instead of executing one at a time. 
		That may give the driver more opportunities to optimize.
		*/
		vkFreeCommandBuffers(_instance->GetDevice(), commandPool, 1, &commandBuffer);
	}

void DestroyBuffer(BufferInformation& bufferInfo)
{
    VmaAllocation allocation = buffers[bufferInfo.buffer];
    vmaDestroyBuffer(*alloc, bufferInfo.buffer, allocation);
    buffers.erase(bufferInfo.buffer);
}

ImageInformation CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage){
    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = tiling;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //Samples is related to multisampling???
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags = 0;
    

    VmaAllocationCreateInfo vmaAllocCreateInfo = {};
    vmaAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    ImageInformation imgInfo = {};
    VmaAllocation vmaAlloc = {};
    VmaAllocationInfo allocInfo = {};

    vmaCreateImage(*alloc, &createInfo, &vmaAllocCreateInfo, &imgInfo.image, &vmaAlloc, nullptr);

    imgInfo.memoryProperties = MemProps::DEVICE;
    imgInfo.width = width;
    imgInfo.height = height;

    images.insert(pair<VkImage, VmaAllocation>(imgInfo.image, vmaAlloc));
    return imgInfo;
}

void CopyBufferToImage(BufferInformation &srcBuffer, ImageInformation &dstImage, uint32_t width, uint32_t height, VkCommandBuffer &commandBuffer) {
    //Specify which part of the buffer will be copied to which part of the image
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };
    
    vkCmdCopyBufferToImage(
        commandBuffer,
        srcBuffer.buffer,
        dstImage.image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}

void DestroyImage(ImageInformation& imageInfo){
    VmaAllocation allocation = images[imageInfo.image];
    vmaDestroyImage(*alloc, imageInfo.image, allocation);
    images.erase(imageInfo.image);
}

char* GetMemoryState()
{
    throw std::runtime_error("NotImplementedException(GetMemoryState): \"Lel get rekt son\"");
}

}; // namespace DeviceMemoryManager