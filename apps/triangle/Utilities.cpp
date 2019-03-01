#include "Utilities.h"


#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>


Mesh* Utilities::MakeCylinder(float radius, float height, int faces){
    vec3 pos = vec3(0.0f,0.0f,radius);
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<uint16_t> indices;
    float angle = 0.0f;
    float step = radians(360.0f/faces);
    //Top and bottom center vertices
    vertices.push_back(vec3(0,height/2.0f,0));
    normals.push_back(vec3(0,1,0));
    vertices.push_back(vec3(0,-height/2.0f,0));
    normals.push_back(vec3(0,-1,0));

    //Create vertices for the top and bottom of the cylinder
    for(int i = 0; i <= faces; i++){
        vec3 newPos = rotate(pos, angle, vec3(0,1,0));
        vertices.push_back(newPos+vec3(0,height/2.0f,0));
        normals.push_back(vec3(0,1,0));
        vertices.push_back(newPos-vec3(0,height/2.0f,0));
        normals.push_back(vec3(0,-1,0));
        angle += step;
    }
    for(uint32_t i = 2; i < vertices.size()-2; i += 2) {
        //Cylinder top triangles
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 2);

        //Cylinder bottom triangles (inverse order to be rendered from bottom view)
        indices.push_back(1);
        indices.push_back(i + 3);
        indices.push_back(i + 1);
    }
    size_t x = vertices.size();
    //Create all vertices
    for(int i = 0; i <= faces; i++){
       vec3 newPos = rotate(pos, angle, vec3(0,1,0));
       vertices.push_back(newPos+vec3(0,height/2.0f,0));
       vec3 n = normalize(newPos);
       normals.push_back(n);
       //cout << n.x << " " << n.y << " " << n.z  << " Lenght: " << length(n) << endl;
       vertices.push_back(newPos-vec3(0,height/2.0f,0));
       normals.push_back(n);
       angle += step;
    }
    //Calculate indices and normals
    for(size_t i = x; i < vertices.size()-2; i += 2){
        //Cylinder side triangles
        indices.push_back(i);
        indices.push_back(i+1);
        indices.push_back(i+2);

        indices.push_back(i+1);
        indices.push_back(i+3);
        indices.push_back(i+2);

    }
    Mesh* cylinder = new Mesh();
    for(uint32_t i = 0; i < vertices.size(); i++){
        Vertex v = {vertices[i], vec3(0.5f), normals[i]};
        cylinder->vertices.push_back(v);
    }
    cylinder->indices = indices;
    return cylinder;
}

Texture Utilities::CreateTexture(uint32_t width, uint32_t height, unsigned char *pixels, uint32_t byteSize, VkCommandBuffer commandBuffer, VkDevice device){
    DeviceMemoryManager::BufferInformation stagingBuffer;
    DeviceMemoryManager::CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, DeviceMemoryManager::MemProps::HOST, byteSize, stagingBuffer);
    DeviceMemoryManager::CopyDataToBuffer(stagingBuffer, pixels);
    
    DeviceMemoryManager::ImageInformation texture = DeviceMemoryManager::CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    TransitionImageLayout(texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandBuffer);
    DeviceMemoryManager::CopyBufferToImage(stagingBuffer, texture, width, height, commandBuffer);
    TransitionImageLayout(texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandBuffer);

    DeviceMemoryManager::DestroyBuffer(stagingBuffer);
    VkImageView imageView = CreateImageView(texture, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, device);
    VkSampler sampler = CreateSampler(device); //Unnecessary to make one per texture since it is independent and can be used for all
    return {imageView, sampler, texture};
}

void Utilities::TransitionImageLayout(DeviceMemoryManager::ImageInformation imageInfo, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer &commandBuffer) {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		//Old layout can be set to VK_IMAGE_LAYOUT_UNDEFINED if the content of the image is unimportant
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
		//Next two are used if transferring queue family ownership. Otherwise must be set to VK_QUEUE_FAMILY_IGNORED
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//image and subresourceRange specify the affected image and its specific parts.
        barrier.image = imageInfo.image;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (HasStencilComponent(format)) {
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
}

bool Utilities::HasStencilComponent(VkFormat format) {
    	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkImageView Utilities::CreateImageView(DeviceMemoryManager::ImageInformation &imageInfo, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device) {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = imageInfo.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

VkSampler Utilities::CreateSampler(VkDevice device) {
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    VkSampler sampler;
    if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a sampler!");
    }
    return sampler;
}