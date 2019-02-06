#pragma once
#include <GLFW\glfw3.h>

#include <string>

struct WindowSetupInfo
{
	std::string title;
	int width;
	int height;
};

struct WindowInfo
{

};

class PlatformLayer
{
public:
	PlatformLayer();
	~PlatformLayer();
	WindowInfo OpenWindow(WindowSetupInfo info);
	void CloseWindow(WindowInfo info);
};

