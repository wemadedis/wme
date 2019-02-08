#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <string>

namespace RTE_PL
{
	enum class RTEWindowStatus{SUCCES, FAIL};
	struct WindowInfo
	{
		RTEWindowStatus status;
		std::string title;
		int width;
		int height;

	};
	WindowInfo OpenWindow(std::string title, int width, int height);
}


