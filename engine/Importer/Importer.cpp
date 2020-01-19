#include "rte/Importer.hpp"

#include "rte/RTEConfig.hpp"
#include "rte/Utility.hpp"
#include "rte/AssimpMeshImporter.hpp"
#include "rte/STBITextureImporter.hpp"

namespace RTE::Importing
{
Rendering::Mesh ImportMesh(std::string file)
{
    RTEConfig *config = RTEConfig::GetInstance();

    IMeshImporter *meshImporter = nullptr;
    switch (config->ImportConfig.MeshImporter)
    {
    case ImportConfig::MeshImporter::ASSIMP:
    {
        meshImporter = new AssimpMeshImporter();
    }
    break;
    case ImportConfig::MeshImporter::WME:
    {
        Error("WME importer not implemented");
    }
    break;
    default:
    {
        Error("Unknown mesh importer");
    }
    }

    Rendering::Mesh mesh = meshImporter->ImportMesh(file);
    return mesh;
}

Rendering::Texture ImportTexture(std::string file)
{
    STBITextureImporter importer;

    Rendering::Texture texture = importer.ImportFromFile(file);

    return texture;
}
} // namespace RTE::Importing
