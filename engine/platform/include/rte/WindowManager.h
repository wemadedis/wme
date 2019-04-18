#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include "rte/Platform.h"

namespace RTE::Platform
{

void DestroyWindow(RTEWindow *window);
void Terminate();

void WaitEvents();
void PollEvents();
bool ShouldClose(RTEWindow *window);

struct WindowBinding
{
  RTEWindow *RTEWindow;
  GLFWwindow *GLFWWindow;
};

class WindowManager
{
public:
  RTEWindow *OpenWindow(int width, int height, std::string title);
  static WindowManager *GetInstance();
  void CreateSurface(
    VkInstance instance,
    VkSurfaceKHR &surface);
  std::vector<const char *> GetRequiredExtensions();
  static int GetKey(int key);
private:
  // Instance for singleton pattern
  static WindowManager *_instance;
  // Creates a Vulkan surface a given GLFW window


  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  // Returns the VulkanExtensions required for GLFW to operate
  
  
  // Callback function called when the size of the window is changed
  static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
  std::vector<WindowBinding *> _activeWindows;
  WindowBinding *_window;

  // Private constructors for singleton pattern
  WindowManager();
  ~WindowManager();
};
}; // namespace RTE::Platform
