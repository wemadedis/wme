#pragma once

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "rte/GlmWrapper.hpp"

#include "rte/RenderStructs.h"
#include "rte/MeshImporter.hpp"

namespace RTE::Importing
{

class AssimpMeshImporter : public MeshImporter
{
private:
    static RTE::Rendering::Material ConvertMaterial(aiMaterial *material);
    static float ColorAverage(aiColor4D col, float numChannels = 4);

    static MissingImportData HandleNode(
        RTE::Rendering::Mesh *root,
        const aiScene *scene,
        aiNode *node,
        RTE::Rendering::Transform t);

    static MissingImportData HandleMesh(
        RTE::Rendering::Mesh *root,
        aiMesh *mesh,
        RTE::Rendering::Transform t);

    /**
     * @brief 
     *  Extracts the position, scalar, and rotation information from this node, and converts it 
     *  into a #RTE::Rendering::Transform. 
     * 
     * @param node Source of transformation data
     * @return #RTE::Rendering::Transform Produced by extracting information from @p node.
     */
    static RTE::Rendering::Transform GetTransform(aiNode *node);

public:
    Rendering::Mesh ImportMesh(std::string path) override;
};

} // namespace RTE::Importing
