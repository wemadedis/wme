#include "rte/EntryPoint.h"
#include "rte/ModelImporter.h"
#include "RTE.h"
#include <sstream>

void Initialize()
{
    using namespace RTE::Import;
    std::ostringstream stringStream;
    stringStream << ENGINE_ASSET_DIR << "models/cube.obj";
    std::string cubePath = stringStream.str();

    RTE::Renderer::Mesh mesh = ModelImporter::ImportMesh(cubePath.c_str());
}