#include <iostream>
#include <vector>

#include "rte/Renderer.h"
#include "rte/WindowManager.h"

namespace RTE::Platform
{

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

void WindowManager::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    GetInstance()->_renderingManager->FrameResized(width, height);
}

bool WindowManager::ShouldClose()
{
    return glfwWindowShouldClose(Window);
}

// Initialize WindowManager static instance
WindowManager *WindowManager::_instance = nullptr;

void WindowManager::CreateSurface(
    VkInstance instance,
    VkSurfaceKHR &surface)
{
    if (glfwCreateWindowSurface(instance, Window, nullptr, &surface) != VK_SUCCESS)
    {
        throw RTEException("failed to create window surface!");
    }
}

void WindowManager::SetRenderingManager(Rendering::RenderingManager *rm)
{
    _renderingManager = rm;
}

WindowManager::WindowManager(RTEConfig &config)
{
    _instance = this;
    OpenWindow(
        config.WindowConfig.WindowWidth,
        config.WindowConfig.WindowHeight,
        config.WindowConfig.WindowName);
    SetConsoleTitleA("Egg asdalsjdkasld");
    SetupGLFWCallbacks();
}

void WindowManager::Update(float deltaTime)
{
    glfwPollEvents();
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

void WindowManager::OpenWindow(
    int width,
    int height,
    std::string title)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(Window, this);
    glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);
}

int WindowManager::GetKey(int key)
{
    WindowManager *manager = GetInstance();
    return glfwGetKey(manager->Window, key);
}

void WindowManager::RegisterMouseButtonCallback(MouseButtonCallback cb)
{
    _mbCallbacks.push_back(cb);
}

void WindowManager::RegisterMouseWheelCallback(MouseWheelCallback cb)
{
    _mwCallbacks.push_back(cb);
}

void WindowManager::RegisterMousePositionCallback(MousePositionCallback cb)
{
    _mpCallbacks.push_back(cb);
}

void WindowManager::RegisterKeyCallback(KeyCallback cb)
{
    _keyCallbacks.push_back(cb);
}

void WindowManager::RegisterCharCallback(CharCallback cb)
{
    _charCallbacks.push_back(cb);
}

void WindowManager::SetupGLFWCallbacks()
{
    glfwSetMouseButtonCallback(Window, [](GLFWwindow*, int b, int a, int){
        auto wm = WindowManager::GetInstance();
        for(uint32_t cbIndex = 0; cbIndex < wm->_mbCallbacks.size(); cbIndex++)
        {
            wm->_mbCallbacks[cbIndex](b,a);
        }
    });

    glfwSetScrollCallback(Window, [](GLFWwindow*, double x, double y){
        auto wm = WindowManager::GetInstance();
        for(uint32_t cbIndex = 0; cbIndex < wm->_mwCallbacks.size(); cbIndex++)
        {
            wm->_mwCallbacks[cbIndex](x,y);
        }
    });

    glfwSetCursorPosCallback(Window, [](GLFWwindow*, double x, double y){
        auto wm = WindowManager::GetInstance();
        for(uint32_t cbIndex = 0; cbIndex < wm->_mpCallbacks.size(); cbIndex++)
        {
            wm->_mpCallbacks[cbIndex](x,y);
        }
    });

    glfwSetKeyCallback(Window, [](GLFWwindow*, int key, int, int action, int){
        auto wm = WindowManager::GetInstance();
        for(uint32_t cbIndex = 0; cbIndex < wm->_keyCallbacks.size(); cbIndex++)
        {
            wm->_keyCallbacks[cbIndex](key,action);
        }
    });

    glfwSetCharCallback(Window, [](GLFWwindow*, unsigned int c){
        auto wm = WindowManager::GetInstance();
        for(uint32_t cbIndex = 0; cbIndex < wm->_charCallbacks.size(); cbIndex++)
        {
            wm->_charCallbacks[cbIndex](c);
        }
    });
}

WindowManager *WindowManager::GetInstance()
{
    assert(WindowManager::_instance);
    return WindowManager::_instance;
}
WindowManager::~WindowManager()
{
}
}; // namespace RTE::Platform