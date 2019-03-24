#include "rte/ModelImporter.h"

#include <vector>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include "rte/RenderStructs.h"
#include "Defs.h"
#include "rte/ImportException.h"

namespace RTE::Import
{

inline float ModelImporter::ColorAverage(aiColor4D col, float numChannels)
{
    return (col.r + col.g + col.b) / numChannels;
}

RTE::Render::Material ModelImporter::ConvertMaterial(aiMaterial *material)
{
    aiColor4D diffuse;
    aiColor4D specular;
    float shininess;
    float reflectivity;

    // Float for data retrieval
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse) &&
        AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular) &&
        AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess) &&
        AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_REFLECTIVITY, &reflectivity))
    {
        using namespace RTE::Render;
        Material mat;
        mat.Diffuse = ColorAverage(diffuse);
        mat.Specular = ColorAverage(specular);
        mat.Shininess = shininess;
        mat.Reflectivity = reflectivity;
        return mat;
    }
    else
    {
        throw ImportException();
    }
}

/**
 * @brief 
 *  Extracts the position, scalar, and rotation information from this node, and converts it 
 *  into a #Transform. 
 * 
 * @param node Source of transformation data
 * @return Transform Produced by extracting from #node
 */
RTE::Render::Transform ModelImporter::GetTransform(aiNode *node)
{
    aiQuaternion rot;
    aiVector3D scale, pos;
    node->mTransformation.Decompose(scale, rot, pos);
    glm::fquat q = glm::fquat(rot.w, rot.x, rot.y, rot.z);

    RTE::Render::Transform t(
        glm::vec3(pos.x, pos.y, pos.z),
        glm::vec3(scale.x, scale.y, scale.z),
        glm::eulerAngles(q));
    return t;
}

MissingImportData ModelImporter::HandleNode(
    RTE::Render::Mesh *mesh,
    const aiScene *scene,
    aiNode *node,
    RTE::Render::Transform t)
{
    MissingImportData missingInfo = MissingImportData::NONE;

    RTE::Render::Transform newTransform = t + GetTransform(node);

    // Handle all meshes in the node
    for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++)
    {
        missingInfo |= HandleMesh(mesh, scene->mMeshes[node->mMeshes[meshIndex]], t);
    }

    // Recursively handle child nodes
    for (u32 childIndex = 0; childIndex < node->mNumChildren; childIndex++)
    {
        missingInfo |= HandleNode(mesh, scene, node->mChildren[childIndex], t);
    }
    return missingInfo;
}

inline glm::vec2 ConvertTexture(aiVector3D *tex)
{
    return glm::vec2(tex->x, tex->y);
}

inline glm::vec3 ConvertVector3(aiVector3D aiV)
{
    return glm::vec3(aiV.x, aiV.y, aiV.z);
}

inline glm::vec4 ConvertColor(aiColor4D *aiC)
{
    return glm::vec4(aiC->r, aiC->g, aiC->b, aiC->a);
}

RTE::Render::Vertex ConvertVertex(aiMesh *mesh, u32 vertexIndex)
{
    RTE::Render::Vertex v;
    v.pos = ConvertVector3(mesh->mVertices[vertexIndex]);
    v.normal = ConvertVector3(mesh->mNormals[vertexIndex]);
    v.color = ConvertColor(mesh->mColors[vertexIndex]);
    v.texCoord = ConvertTexture(mesh->mTextureCoords[vertexIndex]);
    return v;
}

MissingImportData ModelImporter::HandleMesh(
    RTE::Render::Mesh *mesh,
    aiMesh *aiMesh,
    RTE::Render::Transform t)
{
    MissingImportData missingInfo = MissingImportData::NONE;

    // TODO: (danh 22/03 16:25): Use this
    //int indexToAdd = mesh->Indices.size();
    for (u32 vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++)
    {
        RTE::Render::Vertex v;
        v = ConvertVertex(aiMesh, vertexIndex);
        //glm::translate(v.pos) * glm::scaleglm::rotate
        // TODO: (danh 22/03 15:02): Convert vertex to parent global space and add to mesh
    }
    return missingInfo;
}

RTE::Render::Mesh ModelImporter::ImportMesh(const char *filename)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);

    // EXTEND: (danh 22/03 13:30): Implement better support for nested meshes

    RTE::Render::Mesh mesh;
    RTE::Render::Transform t;
    // TODO: (danh 22/03 13:42): Add material

    //mesh.Material = ConvertMaterial();
    MissingImportData missingInfo;
    for (u64 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        missingInfo |= HandleMesh(&mesh, scene->mMeshes[meshIndex], t);
    }

    missingInfo |= HandleNode(&mesh, scene, scene->mRootNode, t);

    return mesh;
}
}; // namespace RTE::Import