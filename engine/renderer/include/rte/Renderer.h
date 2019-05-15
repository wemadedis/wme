#pragma once

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
#include "rte/LineDebugDrawModule.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace RTE::Rendering
{

typedef std::function<void(VkSurfaceKHR &surface, VkInstance instance)> SurfaceBindingFunc;

/**
 * @brief Render mode used to set the renderer to use either rasterization, or ray tracing.
 */
enum class RenderMode
{
    RASTERIZE,
    RAYTRACE
};

//Settings related to things such as resolution, background colour, etc. (dunno what more)

struct MeshInfo;
struct TextureInfo;

/**
 * @brief Struct encapsulating the necessary data needed by the renderer to initialize.
 */
struct RendererInitInfo
{
    /**
     * @brief The application name.
     * 
     */
    const char *ApplicationName = "";
    /**
     * @brief The required vulkan extensions to be enabled by the renderer.
     */
    std::vector<const char *> extensions;
    /**
     * @brief The initial width/height of the surface to be rendered to.
     */
    uint32_t Width, Height;
    /**
     * @brief Pointer to a surface binding function, used to bind the surface of a window to the renderer.
     */
    SurfaceBindingFunc BindingFunc;
    /**
     * @brief Boolean indicating if the renderer should enable ray tracing.
     */
    bool RayTracingOn = false;
};

class Renderer
{

private:
    /**
     * @brief The initialization information.
     */
    RendererInitInfo _initInfo;
    /**
     * @brief The graphics pipeline used for rasterization.
     */
    GraphicsPipeline *_pipeline = nullptr;
    /**
     * @brief The render pass used to structurally record draw commands.
     */
    RenderPass *_renderPass = nullptr;
    /**
     * @brief The instance used to initialize Vulkan by the renderer
     */
    Instance *_instance = nullptr;
    /**
     * @brief The swap chain containing information about the frame buffers to be drawn to.
     */
    SwapChain *_swapChain = nullptr;
    /**
     * @brief The command buffer manager used to allocate buffers, create single time buffers, and submitting them.
     */
    CommandBufferManager *_commandBufferManager = nullptr;
    /**
     * @brief Image manager, used to create the necessary Vulkan image resource information for frame buffers and textures.
     */
    ImageManager *_imageManager = nullptr;
    /**
     * @brief The descriptor manager, used to define and allocate descriptor sets for the data to be accessed by the shaders.
     */
    DescriptorManager *_descriptorManager = nullptr;
    /**
     * @brief The device memory manager, used to allocate and manage all memory used by Vulkan, both on host or device side. 
     */
    DeviceMemoryManager *_deviceMemoryManager = nullptr;

    /**
     * @brief The current frame width, set initially through the initialization information. Can be modified by the frame resize callback function.
     */
    uint32_t _frameWidth = 0;
    /**
     * @brief The current frame height, set initially through the initialization information. Can be modified by the frame resize callback function.
     */
    uint32_t _frameHeight = 0;
    /**
     * @brief State informing the renderer if the resize callback function has been called.
     */
    bool _frameChanged = false;

    /**
     * @brief Struct encapsulating all global rendering data used in the shaders
     */
    GlobalUniformData _globalUniform;
    /**
     * @brief Buffer information for the global uniform.
     */
    BufferInformation _globalUniformBuffer;

    /**
     * @brief Vector of all mesh rendering data uploaded through the UploadMesh function. 
     */
    std::vector<MeshInfo> _meshes;
    /**
     * @brief Vector of all texture rendering data uploaded through the UploadTexture function. 
     */
    std::vector<TextureInfo> _textures;
    /**
     * @brief Vector of all instance rendering data created through the CreateMeshInstance function. 
     */
    std::vector<MeshInstance> _meshInstances;
    /**
     * @brief Vector of all point lights used in rendering. 
     */
    std::vector<PointLight> _pointLights;
    /**
     * @brief Vector of all directional lights used in rendering. 
     */
    std::vector<DirectionalLight> _directionalLights;

    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;

    size_t _currentFrame = 0;

    /**
     * @brief Current render mode.
     */
    RenderMode _renderMode;
    /**
     * @brief State on ray tracing capabilities.
     */
    bool _rtxOn = false;
    /**
     * @brief If ray tracing is enabled, this struct contains all details on related properties.
     */
    VkPhysicalDeviceRayTracingPropertiesNV _rtProperties = {};
    /**
     * @brief Acceleration structure used for ray tracing. Nullptr if ray tracing is disabled.
     */
    AccelerationStructure *_accelerationStructure = nullptr;
    /**
     * @brief The ray tracing pipeline. Nullptr if ray tracing is disabled.
     */
    GraphicsPipeline *_pipelineRT = nullptr;
    /**
     * @brief Shader binding table used for ray tracing. Not allocated if ray tracing is disabled.
     */
    BufferInformation _shaderBindingTable;
    /**
     * @brief Buffer containing all data about instances created for the acceleration structure. Not allocated if ray tracing is disabled.
     */
    BufferInformation _instanceBuffer;

    /**
     * @brief GUI module used to draw the graphical interface, if set by the user.
     */
    GUI::GUIModule *_guiModule = nullptr;

    /**
     * @brief Line debug draw module used to draw lines.
     */
    LineDebugDrawModule *_lineModule = nullptr;

    GraphicsPipeline *_linePipeline = nullptr;
    BufferInformation _lineBuffer = {};

    /**
     * @brief Called at renderer creation, instantiating the necessary structure before the user can upload rendering data.
     */
    void Initialize();
    /**
     * @brief Function used to record the render pass for rasterization
     */
    void RecordRenderPass();
    /**
     * @brief Function used to record the render pass for ray tracing
     */
    void RecordRenderPassRT();
    /**
     * @brief Creates the necessary semaphores and fences to synchronize the device with the host.
     */
    void CreateSyncObjects();
    /**
     * @brief Function used to deallocate all data related to the swap chain.
     */
    void CleanupSwapChain();
    /**
     * @brief Function used to recreate all data related to the swap chain.
     */
    void RecreateSwapChain();
    /**
     * @brief Uploads the global uniform data to the device.
     */
    void UploadGlobalUniform();
    /**
     * @brief Creates an empty texture for instances with no texture bound.
     */
    void CreateEmptyTexture();

    /**
     * @brief Gets the properties on ray tracing capabilities of the device
     */
    void GetRTProperties();
    /**
     * @brief Allocates memory and uploads the shader binding table data.
     */
    void CreateShaderBindingTable();

public:
    /**
     * @brief Handle to the empty texture
     */
    static TextureHandle EMPTY_TEXTURE;
    /**
     * @brief Function determining the ray tracing capabilities of the device. Should used before renderer creation.
     */
    static bool IsRaytracingCapable();

    /**
     * @brief Default contructor for the renderer.
     * 
     * @param info The renderer initialization information, filled out by the user.
     * @return true Bullet documentation doesn't specify
     * @return false Bullet documentation doesn't specify
     */
    Renderer(RendererInitInfo info);

    /**
     * @brief Contructor for the renderer with a GUI module.
     * 
     * @param info The renderer initialization information, filled out by the user.
     * @param guiModule A pointer to a GUI module used for rendering the graphical interface.
     */
    Renderer(RendererInitInfo info, GUI::GUIModule *guiModule);

    /*
     * @brief To be called when the frame size changes.
     * @param width The current width of the frame.
     * @param height The current height of the frame.
     */
    void FrameResized(int32_t width, int32_t height);

    /*
     * @brief Renderer frame size
     * @return Returns a glm vector with the frame width and frame height, respectively
     */
    glm::ivec2 GetFrameSize();

    /*
     *   @brief Sets the render mode to make the renderer use either rasterization or raytracing.
     */
    void SetRenderMode(RenderMode mode);
    /**
     * @brief Uploads a mesh to the device.
     * @param mesh The mesh data.
     * @return Returns a mesh handle. Must be managed by the user.
     */
    MeshHandle UploadMesh(Mesh &mesh);

    /**
     * @brief Creates a mesh instance based on an uploaded mesh
     * @param mesh The mesh handle.
     * @return Returns a mesh instance handle. Must be managed by the user.
     */
    MeshInstanceHandle CreateMeshInstance(MeshHandle mesh);

    /**
     * @brief Sets the Mesh Instance Properties.
     * 
     * @param instance The mesh instance handle.
     * @param modelMatrix The model matrix of the instance.
     * @param mat The material of the instance.
     * @param texture The texture of the instance.
     * @param mesh The mesh of the instance.
     */
    void SetMeshInstanceProperties(MeshInstanceHandle instance, glm::mat4 &modelMatrix, Material &mat, TextureHandle texture, MeshHandle mesh);

    /**
     * @brief Binds a mesh to a mesh instance. Used to swap between different meshes.
     * @param mesh The mesh handle.
     * @param instance The mesh instance handle.
     */
    void BindMeshToInstance(MeshHandle mesh, MeshInstanceHandle instance);

    /**
     * @brief Uploads a texture to the device.
     * @param texture The texture image data.
     * @return Returns a texture handle. Must be managed by the user.
     */
    TextureHandle UploadTexture(Texture &texture);

    /**
     * @brief Binds a texture to a mesh instance. Used to swap between different textures.
     * @param texture The texture handle.
     * @param instance The mesh instance handle.
     */
    void BindTextureToMeshInstance(TextureHandle texture, MeshInstanceHandle instance);
    /**
     * @brief To be called after all necessary data has been uploaded. Creates all necessary information for the renderer to render the data.
     */
    void Finalize();

    /**
     * @brief Issue the renderer to draw a frame. If called again, the renderer waits until the _minFrameTime since last call has been reached.
     */
    void Draw();

    /**
     * @brief Sets the material of a mesh instance
     * @param instance The mesh instance handle.
     * @param mat The material.
     */
    void SetInstanceMaterial(MeshInstanceHandle instance, Material &mat);

    /**
     * @brief Sets the material of a mesh instance
     * @param instance The mesh instance handle.
     * @param mat The material.
     */
    void SetInstanceTransform(MeshInstanceHandle instance, glm::mat4 &modelMatrix);

    /**
     * @brief Adds a directional light to be used in rendering.
     * @param light The directional light data.
     * @return Returns a directional light handle. Must be managed by the user.
     */
    DirectionalLightHandle AddDirectionalLight(DirectionalLight light);

    /**
     * @brief Adds a point light to be used in rendering.
     * @param light The point light data.
     * @return Returns a point light handle. Must be managed by the user.
     */
    PointLightHandle AddPointLight(PointLight light);

    /**
     * @brief Sets properties of a previously added directional light.
     * @param light The directional light handle.
     * @param mutator A lambda function used to modify the directional light properties.
     */
    void SetDirectionalLightProperties(DirectionalLightHandle light, std::function<void(DirectionalLight &)> mutator);

    /**
     * @brief Sets properties of a previously added point light.
     * @param light The point light handle.
     * @param mutator A lambda function used to modify the point light properties.
     */
    void SetPointLightProperties(PointLightHandle light, std::function<void(PointLight &)> mutator);

    /**
     * @brief Sets the clear color after each frame draw. 
     * @param color The clear color.
     */
    void SetClearColor(glm::vec4 color);
    /**
     * @brief Sets the camera used to provide the necessary information for rendering. 
     * @param camera The camera data.
     */
    void SetCamera(Camera camera);

    void SetLineDebugDraw(LineDebugDrawModule *module);

};

}; // namespace RTE::Rendering