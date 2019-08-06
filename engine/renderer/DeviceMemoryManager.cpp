#include "DeviceMemoryManager.h"
#define VMA_IMPLEMENTATION

#include <vk_mem_alloc.h>

#include "RTUtilities.h"

using namespace std;

namespace RTE::Rendering
{

uint32_t DeviceMemoryManager::GetMemoryType(VkMemoryRequirements& memoryRequiriments, VkMemoryPropertyFlags memoryProperties)
{
    uint32_t result = 0;
    for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < VK_MAX_MEMORY_TYPES; ++memoryTypeIndex)
    {
        if (memoryRequiriments.memoryTypeBits & (1 << memoryTypeIndex))
        {
            if ((_physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & memoryProperties) == memoryProperties)
            {
                result = memoryTypeIndex;
                break;
            }
        }
    }
    return result;
}
    
DeviceMemoryManager::DeviceMemoryManager(Instance *instance, CommandBufferManager *commandBufferManager)
{
    _allocator = (VmaAllocator *)malloc(sizeof(VmaAllocator));
    VmaAllocatorCreateInfo info = {};
    _instance = instance;
    _cmdbManager = commandBufferManager;
    info.physicalDevice = _instance->GetPhysicalDevice();
    info.device = _instance->GetDevice();
    vmaCreateAllocator(&info, _allocator);
    vkGetPhysicalDeviceMemoryProperties(_instance->GetPhysicalDevice(), &_physicalDeviceMemoryProperties);
}

void DeviceMemoryManager::CreateBuffer(VkBufferUsageFlags bufferUsage, MemProps props, size_t size, Buffer& bufferInformation)
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

    VkResult result = vmaCreateBuffer(*_allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("(DeviceMemoryManager) Failed to create buffer!");
    }
    _buffers.insert(pair<VkBuffer, VmaAllocation>(buffer, allocation));
    
    bufferInformation.bufferUsage = bufferUsage;
    bufferInformation.memoryProperties = props;
    bufferInformation.size = size;
    bufferInformation.buffer = buffer;
}

void DeviceMemoryManager::CopyDataToBuffer(Buffer& bufferInfo, void* data, uint64_t size){
	void* mapping;//= malloc(bufferInfo.size); //<--------------------------------------- TRIED TO FREE IT AFTER UNMAP, GOT EXCEPTION (IS UNMAP FREEING IT IMPLICITLY??)
    VmaAllocation& allocation = _buffers[bufferInfo.buffer];
    vmaMapMemory(*_allocator, allocation, &mapping); 
    if(size == 0)
    {
        memcpy(mapping, data, bufferInfo.size);
    }
    else
    {
        memcpy(mapping, data, size);
    }
    vmaUnmapMemory(*_allocator, allocation);
}

void DeviceMemoryManager::CopyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, size_t size, VkCommandPool commandPool, VkQueue submitQueue) {
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

void DeviceMemoryManager::DestroyBuffer(Buffer& bufferInfo)
{
    VmaAllocation allocation = _buffers[bufferInfo.buffer];
    vmaDestroyBuffer(*_allocator, bufferInfo.buffer, allocation);
    _buffers.erase(bufferInfo.buffer);
}

ImageMemory DeviceMemoryManager::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage){
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
    ImageMemory imgInfo = {};
    VmaAllocation vmaAlloc = {};
    VmaAllocationInfo allocInfo = {};

    vmaCreateImage(*_allocator, &createInfo, &vmaAllocCreateInfo, &imgInfo.image, &vmaAlloc, nullptr);

    imgInfo.memoryProperties = MemProps::DEVICE;
    imgInfo.width = width;
    imgInfo.height = height;

    _images.insert(pair<VkImage, VmaAllocation>(imgInfo.image, vmaAlloc));
    return imgInfo;
}

void DeviceMemoryManager::CopyBufferToImage(Buffer &srcBuffer, ImageMemory &dstImage) {
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
        dstImage.width,
        dstImage.height,
        1
    };
    VkCommandBuffer commandBuffer = _cmdbManager->BeginCommandBufferInstance();
    vkCmdCopyBufferToImage(
        commandBuffer,
        srcBuffer.buffer,
        dstImage.image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    _cmdbManager->SubmitCommandBufferInstance(commandBuffer, _instance->GetGraphicsQueue());
}

void DeviceMemoryManager::DestroyImage(ImageMemory& imageInfo){
    VmaAllocation allocation = _images[imageInfo.image];
    vmaDestroyImage(*_allocator, imageInfo.image, allocation);
    _images.erase(imageInfo.image);
}


void DeviceMemoryManager::AllocateAccelerationStructureMemory(VkAccelerationStructureNV &AS)
{
    VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo;
    memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
    memoryRequirementsInfo.pNext = nullptr;
    memoryRequirementsInfo.accelerationStructure = AS;
    memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;

    VkMemoryRequirements2 memoryRequirements;
    auto rtutil = RTUtilities::GetInstance();
    rtutil->vkGetAccelerationStructureMemoryRequirementsNV(_instance->GetDevice(), &memoryRequirementsInfo, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.allocationSize = memoryRequirements.memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = GetMemoryType(memoryRequirements.memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkDeviceMemory memory;

    VkResult code = vkAllocateMemory(_instance->GetDevice(), &memoryAllocateInfo, nullptr, &memory);
    //NVVK_CHECK_ERROR(code, L"rt AS vkAllocateMemory");

    VkBindAccelerationStructureMemoryInfoNV bindInfo;
    bindInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
    bindInfo.pNext = nullptr;
    bindInfo.accelerationStructure = AS;
    bindInfo.memory = memory;
    bindInfo.memoryOffset = 0;
    bindInfo.deviceIndexCount = 0;
    bindInfo.pDeviceIndices = nullptr;

    code = rtutil->vkBindAccelerationStructureMemoryNV(_instance->GetDevice(), 1, &bindInfo);
    _accelerationStructures.insert(pair<VkAccelerationStructureNV, VkDeviceMemory>(AS, memory));
}

void DeviceMemoryManager::CreateScratchBuffer(std::vector<VkAccelerationStructureNV> &bot, VkAccelerationStructureNV &top, Buffer &buffer)
{
    auto GetScratchBufferSize = [&](VkAccelerationStructureNV handle)
    {
        VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo;
        memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
        memoryRequirementsInfo.pNext = nullptr;
        memoryRequirementsInfo.accelerationStructure = handle;
        memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_NV;

        VkMemoryRequirements2 memoryRequirements;
        RTUtilities::GetInstance()->vkGetAccelerationStructureMemoryRequirementsNV(_instance->GetDevice(), &memoryRequirementsInfo, &memoryRequirements);

        VkDeviceSize result = memoryRequirements.memoryRequirements.size;
        return result;
    };
    uint64_t size = GetScratchBufferSize(top);
    for(uint32_t botIndex = 0; botIndex < bot.size(); botIndex++)
    {
        uint64_t newSize = GetScratchBufferSize(bot[botIndex]);
        if(newSize > size) size = newSize;
    }
    CreateBuffer(VK_BUFFER_USAGE_RAY_TRACING_BIT_NV, MemProps::DEVICE, size, buffer);

}

}; // namespace DeviceMemoryManager