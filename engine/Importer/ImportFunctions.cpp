#include "rte/ImportFunctions.h"
#include "stb_image.h"
#include "rte/ImportException.h"


#include <fstream>

namespace RTE::Importing
{

const char* ReadFile(const char *filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer.data();
}

RTE::Rendering::Texture ImportTexture(const char * filePath)
{
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels)
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

};