#pragma once

#include "rte/RenderStructs.h"

namespace RTE::Importing
{

/**
 * @brief Interface for all mesh  importers.
 * 
 */
class MeshImporter
{
public:
    virtual Rendering::Mesh ImportMesh(std::string path) = 0;
};

enum class MissingMaterialData
{
    NONE = 1 << 0,
    DIFFUSE = 1 << 1,
    SPECULAR = 1 << 2,
    SHININESS = 1 << 3,
    REFLECTIVITY = 1 << 4,
    TRANSPARENCY = 1 << 5
};

inline MissingMaterialData operator|(MissingMaterialData a, MissingMaterialData b)
{
    return static_cast<MissingMaterialData>(static_cast<int>(a) | static_cast<int>(b));
}

inline MissingMaterialData operator|=(MissingMaterialData a, MissingMaterialData b)
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

} // namespace RTE::Importing
