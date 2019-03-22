#include "rte/EntryPoint.h"
#include "rte/AssetImporter.h"
#include "RTE.h"
#include <sstream>

void Initialize()
{
    std::ostringstream stringStream;
    stringStream << ENGINE_ASSET_DIR << "models/cube.obj";
    std::string cubePath = stringStream.str();

    AssetImporter *assimp = new AssetImporter();
    RTE::Renderer::Mesh mesh = assimp->ImportMesh(cubePath.c_str());
}