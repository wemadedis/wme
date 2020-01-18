#pragma once

#include <string>

#include "rte/RenderStructs.h"

namespace RTE::Importing
{
Rendering::Mesh ImportMesh(std::string filePath);
Rendering::Texture ImportTexture(std::string filePath);
} // namespace RTE::Importing
