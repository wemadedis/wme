#pragma once

#include "rte/Component.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"
#include "rte/TransformComponent.hpp"
#include <string>

namespace RTE::Rendering
{
    class RenderingManager;
}

namespace RTE::StdComponents
{
using namespace RTE::Rendering;
class MeshComponent : public Runtime::Component
{
private:
    /**
     * @brief Pointer to the rendering manager
     */
    RenderingManager *_renderingManager;

    /**
     * @brief Absolute path to the file containing the mesh data
     */
    std::string _meshPath;
    
    /**
     * @brief Absolute path to the file containing the texture image data
     */
    std::string _texturePath;

    /**
     * @brief Transform component of the game object that this component is attached to.
     */
    TransformComponent *_transform;
public:

    /**
     * @brief Material specifying mesh surface properties.
     */
    Material Material = {};
    
    /**
     * @brief Default contructor.
     */
    MeshComponent() = default;

    /**
     * @brief Initializes the mesh component.
     * @param transform Transform component attached to the game object.
     * @param meshFilePath Absolute path to the mesh data fle.
     * @param textureFilePath Absolute path to the texture image file.
     */
    void Initialize(TransformComponent *transform, std::string meshFilePath, std::string textureFilePath = std::string(""));

    /**
     * @brief Sets the mesh of this component.
     * @param meshFilePath The absolute file path.
     */
    void SetMesh(std::string meshFilePath);

    /**
     * @brief Sets the texture to be mapped on the mesh.
     * @param meshFilePath The absolute file path.
     */
    void SetTexture(std::string textureFilePath);

    /**
     * @brief Gets the mesh file path.
     * @return Returns a string containing the file path.
     */
    std::string GetMesh();

    /**
     * @brief Gets the texture file path.
     * @return Returns a string containing the file path.
     */
    std::string GetTexture();

    /**
     * @brief Gets the transform component.
     * @return Returns the transform component.
     */
    TransformComponent* GetTransformComponent();

    virtual void Update(float deltaTime) override;
};

}; // namespace RTE::StdComponents