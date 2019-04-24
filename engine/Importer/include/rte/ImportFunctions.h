#pragma once
#include "rte/RenderStructs.h"

namespace RTE::Importing
{
const char* ReadFile(const char *filename);
RTE::Rendering::Texture ImportTexture(const char * filePath);
};
