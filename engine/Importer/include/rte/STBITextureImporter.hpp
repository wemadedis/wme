#pragma once

#include "rte/RenderStructs.h"

namespace RTE::Importing
{
class STBITextureImporter
{
public:
    Rendering::Texture ImportFromFile(std::string file);
};
} // namespace RTE::Importing
