#pragma once

namespace RTE::Importing
{
class TextureImporter
{
public:
    virtual Rendering::Texture ImportFromFile(std::string file);
};
} // namespace RTE::Importing
