#include "rte/STBITextureImporter.hpp"

#include <stb_image.h>

#include "rte/ImportException.hpp"

namespace RTE::Importing
{
Rendering::Texture STBITextureImporter::ImportFromFile(std::string file)
{
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(file.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw ImportException("Failed to import the texture image!");
    }

    uint32_t imageSize = texWidth * texHeight * 4;
    RTE::Rendering::Texture tex;
    tex.Height = texHeight;
    tex.Width = texWidth;
    tex.Pixels = pixels;
    return tex;
}
} // namespace RTE::Importing