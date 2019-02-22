#include <iostream>

#include "rte/WindowManager.h"

namespace RTE::Platform
{
// Initialize static member of class Box
WindowManager *WindowManager::_instance = nullptr;

RTEWindow *WindowManager::OpenWindow(int width, int height, std::string title)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

    RTEWindow *rteWindow = new RTEWindow();
    rteWindow->Width = width;
    rteWindow->Height = height;
    rteWindow->Title = title;
    rteWindow->WindowResized = false;
    rteWindow->glfwWindow = window;

    WindowBinding *binding = new WindowBinding();
    binding->GLFWWindow = window;
    binding->RTEWindow = rteWindow;
    _activeWindows.push_back(binding);

    return rteWindow;
}

void WindowManager::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto wm = GetInstance();
    auto rteWindow = wm->GetRTEFromGLFW(window);
    rteWindow->WindowResized = true;
}

RTEWindow *WindowManager::GetRTEFromGLFW(GLFWwindow *glfwWindow)
{
    for (int windowIndex = 0; windowIndex < _activeWindows.size(); windowIndex++)
    {
        auto curWindow = _activeWindows.at(windowIndex);
        if (curWindow->GLFWWindow == glfwWindow)
        {
            return curWindow->RTEWindow;
        }
    }
    std::cout << "ERROR: Could not find RTE-GLFW binding" << std::endl;
    return nullptr;
}

WindowManager *WindowManager::GetInstance()
{
    return WindowManager::_instance != nullptr ? WindowManager::_instance : new WindowManager();
}

WindowManager::WindowManager()
{
}
WindowManager::~WindowManager()
{
}
}; // namespace RTE::Platform