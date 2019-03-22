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
    int Width, Height;
    SurfaceBindingFunc BindingFunc;
};

typedef uint16_t MeshHandle;
typedef uint16_t TextureHandle;
typedef uint16_t LightHandle;
typedef uint16_t ShaderHandle;



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
    DeviceMemoryManager *_deviceMemoryManager;

    GlobalUniformData _globalUniform;
    BufferInformation _globalUniformBuffer;
    std::vector<MeshInfo*> _meshes;
    std::vector<TextureInfo> _textures;
    std::vector<DirectionalLight> _dirLights;
    
    std::vector<VkSemaphore> _imageAvailableSemaphores;
	std::vector<VkSemaphore> _renderFinishedSemaphores;
	std::vector<VkFence> _inFlightFences;
    size_t _currentFrame = 0;
    
    
    void Initialize();
    void RecordRenderPass();
    void CreateSyncObjects();
    void CleanupSwapChain();
    void RecreateSwapChain();
public:
/*
Used to bind the window surface to the vulkan instance. Remake into a contructor since it will be a class.
*/
Renderer(RendererInitInfo info);

/*
Sets the render mode to make the renderer use either rasterization or raytracing.
Could be defined in RendererSettings but if we want to change rendering mode at run time, this is necessary.
*/
void SetRenderMode(RenderMode mode);


MeshHandle UploadMesh(Mesh* mesh);

void ClearAllMeshData();

TextureHandle UploadTexture(Texture &texture);

void BindTexture(TextureHandle texture, MeshHandle mesh);

void Finalize();
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
void MarkDirty(MeshHandle mesh);

void SetMeshTransform(MeshHandle mesh, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl);


//LLLLLLLLLIIIIIIIIIIIIIIGGGGGGGGGGGHHHHHHHHHHHHTTTTTTTTTTSSSSSSSSSSSSSSSSS!!!!!!!!!!!!!!!!!!!!!!!!!
LightHandle AddLight(Light light);

void SetLightDirection(LightHandle light, glm::vec3 dir)
{
    _dirLights[light].Direction = dir;
    _globalUniform.Light[0] = _dirLights[light];
    _deviceMemoryManager->CopyDataToBuffer(_globalUniformBuffer, &_globalUniform);
}

void SetCamera(Camera camera);

void UploadShader(Shader shader);

};

    


void CreateInstance(std::string appName, VkInstance *instance, std::vector<const char *> extensions, bool enableValidationLayers);
void SetupDebugCallback(VkInstance instance);
void CreateSurface(VkInstance instance);




};