#include "rte/AssetImporter.h"

#include <vector>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include "rte/RenderStructs.h"
#include "Defs.h"
#include "rte/ImportException.h"

RTE::Renderer::Material AssetImporter::ConvertMaterial(aiMaterial *material)
{
    RTE::Renderer::Material mat;

    // Color for data retrieval
    aiColor4D col;
    float numColorChannels = 4;
    float scalar = 0;
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &col))
    {
        mat.Diffuse = (col.r + col.g + col.b) / numColorChannels;
    }
    else
    {
        throw ImportException();
    }

    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &col))
    {
        mat.Specular = (col.r + col.g + col.b) / numColorChannels;
    }
    else
    {
        throw ImportException(); 
    }

    if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &scalar))
    {
        mat.Shininess = scalar;
    }
    else
    {
        throw ImportException();  
    }

    if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_REFLECTIVITY, &scalar))
    {
        mat.Reflectivity = scalar;
    }
    else
    {
        throw ImportException(); 
    }

    return mat;
}

/**
 * @brief 
 *  Extracts the position, scalar, and rotation information from this node, and converts it 
 *  into a #Transform. 
 * 
 * @param node Source of transformation data
 * @return Transform Produced by extracting from #node
 */
RTE::Renderer::Transform AssetImporter::GetTransform(aiNode *node)
{
    aiQuaternion rot;
    aiVector3D scale, pos;
    node->mTransformation.Decompose(scale, rot, pos);
    glm::fquat q = glm::fquat(rot.w, rot.x, rot.y, rot.z);

    RTE::Renderer::Transform t(
        glm::vec3(pos.x, pos.y, pos.z),
        glm::vec3(scale.x, scale.y, scale.z),
        glm::eulerAngles(q));
    return t;
}

void AssetImporter::HandleNode(
    RTE::Renderer::Mesh *mesh,
    const aiScene *scene,
    aiNode *node,
    RTE::Renderer::Transform t)
{
    RTE::Renderer::Transform newTransform = t + GetTransform(node);

    // Handle all meshes in the node
    for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++)
    {
        HandleMesh(mesh, scene->mMeshes[node->mMeshes[meshIndex]], t);
    }

    // Recursively handle child nodes
    for (u32 childIndex = 0; childIndex < node->mNumChildren;  childIndex++)
    {
        HandleNode(mesh, scene, node->mChildren[childIndex], t);
    }
}

glm::vec2 ConvertTexture(aiVector3D *tex)
{
    return glm::vec2(tex->x, tex->y);
}

glm::vec3 ConvertVector3(aiVector3D aiV)
{
    return glm::vec3(aiV.x, aiV.y, aiV.z);
}

glm::vec4 ConvertColor(aiColor4D *aiC)
{
    return glm::vec4(aiC->r, aiC->g, aiC->b, aiC->a);
}

RTE::Renderer::Vertex ConvertVertex(aiMesh *mesh, u32 vertexIndex)
{
    RTE::Renderer::Vertex v = RTE::Renderer::Vertex();
    v.pos = ConvertVector3(mesh->mVertices[vertexIndex]);
    v.normal = ConvertVector3(mesh->mNormals[vertexIndex]);
    v.color = ConvertColor(mesh->mColors[vertexIndex]);
    v.texCoord = ConvertTexture(mesh->mTextureCoords[vertexIndex]);
    return v;
}

void AssetImporter::HandleMesh (
    RTE::Renderer::Mesh *mesh, 
    aiMesh *aiMesh,
    RTE::Renderer::Transform t)
{
    // TODO: (danh 22/03 16:25): Use this
    //int indexToAdd = mesh->Indices.size();
    for (u32 vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++)
    {
        RTE::Renderer::Vertex v = RTE::Renderer::Vertex();
        v = ConvertVertex(aiMesh, vertexIndex);
        //glm::translate(v.pos) * glm::scaleglm::rotate
        // TODO: (danh 22/03 15:02): Convert vertex to parent global space and add to mesh
    }
}

RTE::Renderer::Mesh AssetImporter::ImportMesh(const char *filename)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);

    // EXTEND: (danh 22/03 13:30): Implement better support for nested meshes
    RTE::Renderer::Mesh mesh;
    RTE::Renderer::Transform t;

    // TODO: (danh 22/03 13:42): Add material
    
    //smesh.Material = ConvertMaterial();

    for (u64 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        HandleMesh(&mesh, scene->mMeshes[meshIndex], t);
    }

    HandleNode(&mesh, scene, scene->mRootNode, t);

    return mesh;
}