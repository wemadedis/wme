#include "PlatformLayer.h"

#include <tuple>

namespace RTE_PL
{
	WindowInfo OpenWindow(std::string title, int width, int height)
	{
		GLFWwindow* window;
		WindowInfo w = WindowInfo();

		/* Initialize the library */
		if (!glfwInit())
			w.status = RTEWindowStatus::FAIL;


		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			w.status = RTEWindowStatus::FAIL;
		}
		return w;
	}
}

