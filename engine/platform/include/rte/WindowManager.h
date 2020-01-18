#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"
#include "rte/RenderingManager.h"

#include <functional>
#include <string>
#include <vector>

namespace RTE::Platform
{
void Terminate();
void WaitEvents();
void PollEvents();

typedef std::function<void(int button, int action)> MouseButtonCallback;
typedef std::function<void(double xoffset, double yoffset)> MouseWheelCallback;
typedef std::function<void(double x, double y)> MousePositionCallback;
typedef std::function<void(int key, int action)> KeyCallback;
typedef std::function<void(unsigned int character)> CharCallback;


class WindowManager : public RTEModule
{
public:
    WindowManager();
    bool ShouldClose();
    void OpenWindow(int width, int height, std::string title);
    static WindowManager *GetInstance();
    void CreateSurface(
        VkInstance instance,
        VkSurfaceKHR &surface);

    // Returns the VulkanExtensions required for GLFW to operate
    std::vector<const char *> GetRequiredExtensions();
    static int GetKey(int key);
    static void CursorHide();
    static void CursorShow();
    void Update(float deltaTime) override;

    GLFWwindow *Window;
    void SetRenderingManager(Rendering::RenderingManager *rm);

    void RegisterMouseButtonCallback(MouseButtonCallback cb);
    void RegisterMouseWheelCallback(MouseWheelCallback cb);
    void RegisterMousePositionCallback(MousePositionCallback cb);
    void RegisterKeyCallback(KeyCallback cb);
    void RegisterCharCallback(CharCallback cb);
    
private:
    // Instance for singleton pattern
    static WindowManager *_instance;
    // Creates a Vulkan surface a given GLFW window
    Rendering::RenderingManager *_renderingManager;

    std::vector<MouseButtonCallback> _mbCallbacks;
    std::vector<MouseWheelCallback> _mwCallbacks;
    std::vector<MousePositionCallback> _mpCallbacks;
    std::vector<KeyCallback> _keyCallbacks;
    std::vector<CharCallback> _charCallbacks;
    void SetupGLFWCallbacks();

    // Callback function called when the size of the window is changed
    static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);

    ~WindowManager();
};
}; // namespace RTE::Platform
