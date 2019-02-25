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

private:
  static WindowManager *_instance;

  static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
  std::vector<WindowBinding *> _activeWindows;
  RTEWindow *GetRTEFromGLFW(GLFWwindow *window);
  WindowManager();
  ~WindowManager();
};
}; // namespace RTE::Platform
