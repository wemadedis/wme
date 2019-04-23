#pragma once

#include <functional>

typedef std::function<void(int width, int height)> FrameResizeCallback;
typedef std::function<void(FrameResizeCallback fcb)> SetFrameResizeCallback;