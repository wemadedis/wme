#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"

#include "rte/FrameCallbackFunctions.hpp"

#include <functional>
#include <string>
#include <vector>

namespace RTE::Platform
{
void Terminate();
void WaitEvents();
void PollEvents();

class WindowManager : public RTEModule
{
public:
    bool ShouldClose();
    void OpenWindow(int width, int height, std::string title);
    static WindowManager *GetInstance();
    void CreateSurface(
        VkInstance instance,
        VkSurfaceKHR &surface);
    std::vector<const char *> GetRequiredExtensions();
    static int GetKey(int key);
    void Update(float deltaTime) override;
    WindowManager(RTE::RTEConfig &config);

    GLFWwindow *Window;
    void SetFrameResizeCallback(FrameResizeCallback fcb);
    FrameResizeCallback GetFrameResizeCallback();

private:
    // Instance for singleton pattern
    static WindowManager *_instance;
    // Creates a Vulkan surface a given GLFW window

    FrameResizeCallback _frameResizeCallback = nullptr;

    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    // Returns the VulkanExtensions required for GLFW to operate

    // Callback function called when the size of the window is changed
    static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);

    ~WindowManager();
};
}; // namespace RTE::Platform
