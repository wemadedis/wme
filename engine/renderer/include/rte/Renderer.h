#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <stdexcept>
#include <vector>

#include "RenderStructs.h"
#include "Utilities.h"
#include "DeviceMemoryManager.h"
#include "GraphicsPipeline.hpp"
#include "RenderPass.hpp"
#include "Instance.hpp"
#include "SwapChain.hpp"
#include "CommandBufferManager.hpp"
#include "ImageManager.hpp"
#include "DescriptorManager.hpp"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


//make a renderer class as well. rename struct and keep it for some additional structs
namespace RTE::Renderer
{
typedef std::function<void(VkSurfaceKHR &surface, VkInstance instance)> SurfaceBindingFunc;
enum RenderMode { RASTERIZE, RAYTRACE };

//Settings related to things such as resolution, background colour, etc. (dunno what more)

struct MeshInfo;
struct TextureInfo;

struct RendererInitInfo
{
    std::vector<const char*> extensions;
    int Widht, Height;
    SurfaceBindingFunc BindingFunc;
};

class Renderer
{
private:
    RendererInitInfo _initInfo;
    GraphicsPipeline *_pipeline;
	RenderPass *_renderPass;
	Instance *_instance;
	SwapChain *_swapChain;
	CommandBufferManager *_commandBufferManager;
	ImageManager *_imageManager;
	DescriptorManager *_descriptorManager;

    std::vector<MeshInfo*> _meshes;
    std::vector<TextureInfo> _textures;
    
    std::vector<VkSemaphore> _imageAvailableSemaphores;
	std::vector<VkSemaphore> _renderFinishedSemaphores;
	std::vector<VkFence> _inFlightFences;
    size_t _currentFrame = 0;
    
    void Initialize();
    void UploadMesh(Mesh* mesh);
    /*
    Upload meshes to the GPU. The meshes can still be used to call the update/other kind of functions.
    Can be made into single mesh upload.
    */
    void UploadMeshes(std::vector<Mesh*> &meshData);
    void RecordRenderPass();
    void CreateSyncObjects();
    void CleanupSwapChain();
    void RecreateSwapChain();
    void CreateTexture(MeshInfo* mesh, const char *imagePath);
public:
/*
Used to bind the window surface to the vulkan instance. Remake into a contructor since it will be a class.
*/
Renderer(RendererInitInfo info, std::vector<Mesh*> &meshData);

/*
Sets the render mode to make the renderer use either rasterization or raytracing.
Could be defined in RendererSettings but if we want to change rendering mode at run time, this is necessary.
*/
void SetRenderMode(RenderMode mode);



/*
Functions to remove individual meshes or just clearing all mesh data.
*/
void RemoveMesh(Mesh* mesh);
void ClearAllMeshData();
/*
Draw the scene based on some info given to the renderer. Does not need to be called renderpass. Though prolly best
In this info struct we define which meshes to render (when for instance frustum culling). LOOK AT SRE 😥
*/
void Draw(RenderPassInfo rpInfo);

/*
In case of raytracing, no need to provide any info to the draw call?? Or just make the previous draw take a pointer, and then pass a nullptr?
*/
void Draw();

/*
Inform the renderer that some property of the mesh has changed, thus forcing it to synchronize the data.
The renderer will the internally handle everything related to this.
*/
void MarkDirty(Mesh* mesh);


//LLLLLLLLLIIIIIIIIIIIIIIGGGGGGGGGGGHHHHHHHHHHHHTTTTTTTTTTSSSSSSSSSSSSSSSSS!!!!!!!!!!!!!!!!!!!!!!!!!
void AddLight(Light light);

void RemoveLight(Light light);

void AddTexture(Texture texture);

void RemoveTexture(Texture texture);

void UploadShader(Shader shader);

void RemoveShader(Shader shader);


};

    


void CreateInstance(std::string appName, VkInstance *instance, std::vector<const char *> extensions, bool enableValidationLayers);
void SetupDebugCallback(VkInstance instance);
void CreateSurface(VkInstance instance);




};