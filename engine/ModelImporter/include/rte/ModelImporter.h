#pragma once

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "rte/RenderStructs.h"

namespace RTE::Importing
{

enum class MisssingMaterialData
{
    NONE = 0,
    DIFFUSE = 1,
    SPECULAR = 2,
    SHININESS = 4,
    REFLECTIVITY = 8,
    TRANSPARENCY = 16
};

inline MisssingMaterialData operator|(MisssingMaterialData a, MisssingMaterialData b)
{
    return static_cast<MisssingMaterialData>(static_cast<int>(a) | static_cast<int>(b));
}

inline MisssingMaterialData operator|=(MisssingMaterialData a, MisssingMaterialData b)
{
    return a | b;
}

enum class MissingImportData
{
    NONE = 0,
    MISSING_VERTICES = 1,
    MISSING_NORMALS = 2,
    MISSING_UVS = 4,
    MISSING_COLORS = 8
};

inline MissingImportData operator|(MissingImportData a, MissingImportData b)
{
    return static_cast<MissingImportData>(static_cast<int>(a) | static_cast<int>(b));
}

inline MissingImportData operator|=(MissingImportData a, MissingImportData b)
{
    return a | b;
}

class ModelImporter
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

    ModelImporter() = default;

  public:
    static RTE::Rendering::Mesh ImportMesh(const char *filename);
};
}; // namespace RTE::Importing