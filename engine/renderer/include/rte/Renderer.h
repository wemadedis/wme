#pragma once

#include <chrono>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "CommandBufferManager.hpp"
#include "DescriptorManager.hpp"
#include "DeviceMemoryManager.h"
#include "GraphicsPipeline.hpp"
#include "ImageManager.hpp"
#include "Instance.hpp"
#include "RenderPass.hpp"
#include "RenderStructs.h"
#include "SwapChain.hpp"
#include "Utilities.h"

//RT
#include "AccelerationStructureRT.h"

#include "rte/GUIModule.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

//make a renderer class as well. rename struct and keep it for some additional structs
namespace RTE::Rendering
{

typedef std::function<void(VkSurfaceKHR &surface, VkInstance instance)> SurfaceBindingFunc;
enum class RenderMode
{
    RASTERIZE,
    RAYTRACE
};

//Settings related to things such as resolution, background colour, etc. (dunno what more)

struct MeshInfo;
struct TextureInfo;

struct RendererInitInfo
{
    std::vector<const char *> extensions;
    uint32_t Width, Height;
    SurfaceBindingFunc BindingFunc;
    int MaxFPS = 60;
    bool RayTracingOn = false;
    glm::vec4 ClearColor = glm::vec4(0.0f);
};

class Renderer
{

    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
    using FpSeconds = std::chrono::duration<float, std::chrono::seconds::period>;

private:
    RendererInitInfo _initInfo;
    GraphicsPipeline *_pipeline = nullptr;
    RenderPass *_renderPass = nullptr;
    Instance *_instance = nullptr;
    SwapChain *_swapChain = nullptr;
    CommandBufferManager *_commandBufferManager = nullptr;
    ImageManager *_imageManager = nullptr;
    DescriptorManager *_descriptorManager = nullptr;
    DeviceMemoryManager *_deviceMemoryManager = nullptr;

    uint32_t _frameWidth = 0;
    uint32_t _frameHeight = 0;
    bool _frameChanged = false;

    GlobalUniformData _globalUniform;
    BufferInformation _globalUniformBuffer;

    std::vector<ShaderInfo> _shaders;

    std::vector<MeshInfo *> _meshes;
    std::vector<TextureInfo> _textures;
    std::vector<MeshInstance> _meshInstances;
    //WARNING: CURRENT STRUCTURE DOES NOT ALLOW TO REMOVE LIGHTS FROM THE LIST AS THE HANDLES CAN BECOME INVALID
    std::vector<PointLight> _pointLights;
    std::vector<DirectionalLight> _directionalLights;

    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    size_t _currentFrame = 0;

    RenderMode _renderMode;
    bool RTXon = false; //TODO: Apply coding style
    VkPhysicalDeviceRayTracingPropertiesNV _rtProperties = {};
    AccelerationStructure *_accelerationStructure = nullptr;
    GraphicsPipeline *_pipelineRT = nullptr;
    BufferInformation _shaderBindingTable;
    ImageInformation _offScreenImageRT;
    VkImageView _offScreenImageView;
    BufferInformation _instanceBuffer;

    const float _minFrameTime;
    TimePoint _lastFrameEnd;


    GUI::GUIModule *_guiModule = nullptr;

    void Initialize();
    void RecordRenderPass();
    void RecordCommandBufferForFrame(VkCommandBuffer commandBuffer, uint32_t frameIndex);
    void RecordCommandBuffersRT();
    void CreateSyncObjects();
    void CleanupSwapChain();
    void RecreateSwapChain();
    void UploadGlobalUniform();
    void CreateEmptyTexture();

    void InitRT();
    void CreateShaderBindingTable();

public:
    static TextureHandle EMPTY_TEXTURE;
    static bool IsRaytracingCapable();
    /*
Used to bind the window surface to the vulkan instance. Remake into a contructor since it will be a class.
*/
    Renderer(RendererInitInfo info);

    Renderer(RendererInitInfo info, GUI::GUIModule *guiModule);

    void FrameResized(int32_t width, int32_t height);

    /**
    * @brief Renderer frame size
    * @return Returns a glm vector with the frame width and frame height, respectively
    */
    glm::ivec2 GetFrameSize();

    /*
Sets the render mode to make the renderer use either rasterization or raytracing.
Could be defined in RendererSettings but if we want to change rendering mode at run time, this is necessary.
*/
    void SetRenderMode(RenderMode mode);

    MeshHandle UploadMesh(Mesh &mesh);

    MeshInstanceHandle CreateMeshInstance(MeshHandle mesh);

    void BindMeshToInstance(MeshHandle mesh, MeshInstanceHandle instance);

    void ClearAllMeshData();

    TextureHandle UploadTexture(Texture &texture);

    void BindTextureToMeshInstance(TextureHandle texture, MeshInstanceHandle instance);

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

    void SetInstanceMaterial(MeshInstanceHandle instance, Material &mat);

    void SetMeshTransform(MeshInstanceHandle mesh, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl);

    //LLLLLLLLLIIIIIIIIIIIIIIGGGGGGGGGGGHHHHHHHHHHHHTTTTTTTTTTSSSSSSSSSSSSSSSSS!!!!!!!!!!!!!!!!!!!!!!!!!
    DirectionalLightHandle AddDirectionalLight(DirectionalLight light);

    PointLightHandle AddPointLight(PointLight light);

    void SetDirectionalLightProperties(DirectionalLightHandle light, std::function<void(DirectionalLight &)> mutator);

    void SetPointLightProperties(PointLightHandle light, std::function<void(PointLight &)> mutator);

    void SetClearColor(glm::vec4 color);

    void SetCamera(Camera camera);

    ShaderHandle UploadShader(Shader shader);
};

void CreateInstance(std::string appName, VkInstance *instance, std::vector<const char *> extensions, bool enableValidationLayers);
void SetupDebugCallback(VkInstance instance);
void CreateSurface(VkInstance instance);

}; // namespace RTE::Rendering