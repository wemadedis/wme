#include "PlatformLayer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <tuple>
PlatformLayer::PlatformLayer()
{
}


PlatformLayer::~PlatformLayer()
{
}

void PlatformLayer::OpenWindow(WindowSetupInfo info)
{

	GLFWwindow* window;

	auto [t, w, h] = info;

	/* Initialize the library */
	if (!glfwInit())
		return;


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(w, h, t.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return;
	}
}