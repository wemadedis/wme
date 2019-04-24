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

class WindowManager : public RTEModule
{
public:
    WindowManager(RTE::RTEConfig &config);
    bool ShouldClose();
    void OpenWindow(int width, int height, std::string title);
    static WindowManager *GetInstance();
    void CreateSurface(
        VkInstance instance,
        VkSurfaceKHR &surface);

    // Returns the VulkanExtensions required for GLFW to operate
    std::vector<const char *> GetRequiredExtensions();
    static int GetKey(int key);
    void Update(float deltaTime) override;

    GLFWwindow *Window;
    void SetRenderingManager(Rendering::RenderingManager *rm);

private:
    // Instance for singleton pattern
    static WindowManager *_instance;
    // Creates a Vulkan surface a given GLFW window
    Rendering::RenderingManager *_renderingManager;

    // Callback function called when the size of the window is changed
    static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);

    ~WindowManager();
};
}; // namespace RTE::Platform
