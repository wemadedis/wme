#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

#include "Utilities.h"

namespace RTE::Rendering
{

struct RendererInitInfo;

/**
 *  @brief Encapsulates and manages Vulkan abstractions used for initialization: VkInstance, VkDevice. Also creates VKQueues used for submitting commands to the GPU.  
 */
class Instance 
{
private:
    /**
     * @brief Vulkan instance handle for the renderer.
     */
    VkInstance _instance;

    /**
     * @brief Vulkan handle to the physical device used by the renderer.
     */
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

    /**
     * @brief Vulkan handle to the logical device based on the physical device.
     */
    VkDevice _device;

    /**
     * @brief Surface used to render on, obtained through the surface binding function.
     */
    VkSurfaceKHR _surface;

    /**
     * @brief The graphics queue, used to submit draw commands.
     */
    VkQueue _graphicsQueue;

    /**
     * @brief The present queue, used to queue an image for presentation.
     */
    VkQueue _presentQueue;

    /**
     * @brief Vulkan handle to a debug callback object.
     */
    VkDebugUtilsMessengerEXT _callback;

    /**
     * @brief State of ray tracing capabilities by the available devices.
     */
    bool _rayTracingCapable = false;

    /**
     * @brief The validation layers used for debugging.
     */
    const std::vector<const char *> validationLayers = 
    { "VK_LAYER_LUNARG_standard_validation" };

    /**
     * @brief The device extentions needed by the renderer.
     */
    std::vector<const char *> _neededDeviceExtensions = 
    { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    /**
     * @brief Creates the Vulkan instance.
     * @param extensions The extenstions provided by the windowing system.
     */
    void CreateInstance(const char* appName, std::vector<const char*> &extensions);

    /**
     * @brief Determines if a physical device meets the requirements of the renderer.
     * @param device The physical device to be evaluated.
     */
    bool DeviceMeetsRequirements(VkPhysicalDevice device);
    
    /**
     * @brief Scans through the physical devices available and picks the optimal one.
     * @param rtRequested If true, the function will search for a device with RT capabilities. If not found, throws an exception.
     */
    void ChoosePhysicalDevice(bool rtRequested);

    /**
     * @brief Creates the logical device.
     */
    void CreateLogicalDevice();

    /**
     * @brief Sets up the debug callback that provides necessary warnings or errors.
     */
    void SetupDebugCallBack();
    
public:
    /**
     * @brief The default constructor.
     * @param extensions The extensions provided by the windowing system.
     * @param surfaceBindingFunction Function that binds the surface of a window with the Vulkan instance.
     * @param isRayTracing If true, enables ray tracing if it is provided by the GPU.
     */ 
    Instance(RendererInitInfo initInfo); 

    ~Instance();
    
    /**
     * @brief Getter for the Vulkan instance
     * @return Returns the Vulkan instance.
     */
    VkInstance GetInstance();
    
    /**
     * @brief Getter for the Vulkan device
     * @return Returns the Vulkan device.
     */
    VkDevice GetDevice();

    /**
     * @brief Getter for the graphics queue.
     * @return Returns the graphics queue.
     */
    VkQueue GetGraphicsQueue();

    /**
     * @brief Getter for the present queue.
     * @return Returns the present queue.
     */
    VkQueue GetPresentQueue();

    /**
     * @brief Getter for the physical device.
     * @return Returns the physical device.
     */
    VkPhysicalDevice GetPhysicalDevice();

    /**
     * @brief Getter for the target surface.
     * @return Returns the Vulkan surface handle.
     */
    VkSurfaceKHR GetSurface();

    /**
     * @brief Finds the optimal depth format supported by the physical device.
     * @return Returns the depth format.
     **/
	VkFormat GetOptimalDepthFormat();

    /**
     * @brief Determines wheter the GPU(s) connected support ray tracing.
     * @return True if there is any GPU supporting ray tracing.
     * @return False if no ray tracing capable GPU is found.
     */
    static bool IsRayTracingCapable();
};

};