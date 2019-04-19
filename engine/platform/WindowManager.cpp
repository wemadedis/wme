#include <iostream>
#include <vector>

#include "rte/WindowManager.h"
#include "rte/Renderer.h"

namespace RTE::Platform
{

void DestroyWindow(RTEWindow* window)
{
    glfwDestroyWindow(window->window);
}
void Terminate()
{
    glfwTerminate();
}

void WaitEvents()
{
    glfwWaitEvents();
}
void PollEvents()
{
    glfwPollEvents();
}

bool ShouldClose(RTEWindow *window)
{
    return glfwWindowShouldClose(window->window);
}

// Initialize WindowManager static instance
WindowManager *WindowManager::_instance = nullptr;

void WindowManager::CreateSurface(
    VkInstance instance,
    VkSurfaceKHR &surface)
{
    if (glfwCreateWindowSurface(instance, _window->GLFWWindow, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

std::vector<const char *> WindowManager::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

RTEWindow *WindowManager::OpenWindow(
    int width, 
    int height, 
    std::string title)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);

    RTEWindow *rteWindow = new RTEWindow();

    VkInstance *instance = new VkInstance;
    VkSurfaceKHR *surface = new VkSurfaceKHR;
    //RTE::Renderer::CreateInstance(title, instance, GetRequiredExtensions(), enableValidationLayers);
    //RTE::Renderer::SetupDebugCallback(*instance);

    rteWindow->Width = width;
    rteWindow->Height = height;
    rteWindow->Title = title;
    rteWindow->WindowResized = false;
    rteWindow->instance = instance;
    rteWindow->window = window;

    glfwSetKeyCallback(window, KeyCallback);
    //CreateSurface(*instance, window, surface);
    rteWindow->surface = surface;

    WindowBinding *binding = new WindowBinding();
    binding->GLFWWindow = window;
    binding->RTEWindow = rteWindow;
    //_activeWindows.push_back(binding);
    _window = binding;
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

    return rteWindow;
}

void WindowManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void WindowManager::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto win = GetInstance()->_window->RTEWindow;
    win->Width = width;
    win->Height = height;
    win->WindowResized = true;
}

int WindowManager::GetKey(int key)
{
    WindowManager* manager = GetInstance();
    return glfwGetKey(manager->_window->GLFWWindow, key);
}

WindowManager *WindowManager::GetInstance()
{
    if (WindowManager::_instance == nullptr)
        _instance = new WindowManager();
    return WindowManager::_instance;
}

WindowManager::WindowManager()
{
}
WindowManager::~WindowManager()
{
}
}; // namespace RTE::Platform