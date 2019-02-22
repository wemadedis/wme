#pragma once
#include <string>
#include <GLFW/glfw3.h>
namespace RTE::Platform
{
struct RTEWindow
{
    int Width;
    int Height;
    std::string Title;
    bool WindowResized;
    GLFWwindow *glfwWindow;
};
} // namespace RTE::Platform
