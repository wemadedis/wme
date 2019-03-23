#pragma once

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "rte/RenderStructs.h"

class AssetImporter
{
  private:
    RTE::Renderer::Material ConvertMaterial(aiMaterial *material);

    void HandleNode(
        RTE::Renderer::Mesh *root,
        const aiScene *scene,
        aiNode *node,
        RTE::Renderer::Transform t);
    void HandleMesh(
        RTE::Renderer::Mesh *root,
        aiMesh *mesh,
        RTE::Renderer::Transform t);

    RTE::Renderer::Transform GetTransform(aiNode *);

  public:
    RTE::Renderer::Mesh ImportMesh(const char *filename);
    AssetImporter() = default;
};