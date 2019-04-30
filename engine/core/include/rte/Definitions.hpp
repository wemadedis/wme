#pragma once

#include <cstdint>
#include <string>
#include <functional>

typedef uint32_t DefaultIdType;
typedef DefaultIdType ComponentId;
typedef DefaultIdType ComponentPoolId;
typedef DefaultIdType GameObjectId;
typedef DefaultIdType CollisionId;

typedef std::string RelativeFilePath;
typedef std::string AbsoluteFilePath;

typedef std::function<void()> GUIDrawFunction;