#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
namespace RTE::Platform
{
struct RTEWindow
{
    int Handle;
    int Width;
    int Height;
    std::string Title;
    bool WindowResized;
    GLFWwindow* window;
    VkInstance* instance;
    VkSurfaceKHR* surface;
};
} // namespace RTE::Platform
