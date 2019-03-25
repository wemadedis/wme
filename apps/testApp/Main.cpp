#include "rte/EntryPoint.h"
#include "rte/ModelImporter.h"
#include "RTE.h"
#include <sstream>

void Initialize()
{
    using namespace RTE::Importing;
    CreateScene();
    std::ostringstream stringStream;
    stringStream << ENGINE_ASSET_DIR << "models/nested.ply";
    std::string cubePath = stringStream.str();

    RTE::Rendering::Mesh mesh = ModelImporter::ImportMesh(cubePath.c_str());
}