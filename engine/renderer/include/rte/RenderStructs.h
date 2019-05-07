// TODO: (danh 22/02 09:57): Place this in appropriate Renderer-library when it exists
#pragma once
#include "rte/GlmWrapper.hpp"
#include <array>
#include <vector>
#include <vulkan/vulkan.h>

namespace RTE::Rendering
{
/**
 * @brief Struct encapsulating vertex data used by meshes.
 */
struct Vertex
{
    /**
     * @brief The XYZ position of the vertex.
     */
    glm::vec3 Position;

    /**
     * @brief The RGBA color of the vertex.
     */
    glm::vec4 Color;

    /**
     * @brief The XYZ normal of the vertex.
     */
    glm::vec3 Normal;

    /**
     * @brief The UV coordinates used for texture mapping.
     */
    glm::vec2 UV;


    /**
     * @brief Gets the Vulkan vertex input binding description, used by the renderer. 
     * @return Returns the corresponing binding description Vulkan struct.
     */
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    /**
     * @brief Gets the Vulkan vertex input attribute description, used by the renderer. 
     * @return Returns the corresponing attribute description Vulkan struct.
     */
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, Position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, Color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, Normal);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, UV);
        return attributeDescriptions;
    }
};

/**
 * @brief Struct encapsulating the camera data used for rendering.
 */
struct Camera
{
    /**
     * @brief The field of view of the camera, given in degrees.
     */
    float FieldOfView;

    /**
     * @brief The distance to the near plane.
     */
    float NearPlane;

    /**
     * @brief The distance to the far plane.
     */
    float FarPlane;

    /**
     * @brief The XYZ position of the camera. Used by the renderer for ray tracing.
     */
    glm::vec3 Position;

    /**
     * @brief A 4x4 matrix used to transform vertices from global to camera space.
     */
    glm::mat4 ViewMatrix;
    
    /**
     * @brief A 4x4 matrix used to  transform vertices from view to clip space.
     */
    glm::mat4 ProjectionMatrix;
};

/**
 * @brief Struct containing image information for texture creation.
 */
struct Texture
{
    unsigned char *Pixels;
    uint16_t Width, Height;
};

/**
 * @brief Struct encapsulating the properties of a mesh surface. 
 */
struct Material
{
    /**
     * @brief The ambient reflection intensity. 
     */
    float Ambient = 0.2f;

    /**
     * @brief The diffuse reflection intensity. 
     */
    float Diffuse = 0.3f;

    /**
     * @brief The specular reflection intensity. 
     */
    float Specular = 0.3f;

    /**
     * @brief The shininess coefficient. 
     */
    float Shininess = 1.0f;

    /**
     * @brief The reflectivity of the surface. Used for ray tracing. 
     */
    float Reflectivity = 0.0f;

    //TODO: need dis?
    /**
     * @brief The transparency of the surface. To be used for ray tracing. 
     */
    float Transparency = 0.0f;

    //TODO: We currently do not use color, and texture should be defined by path (for the rendering manager). Remove these?
    //glm::vec4 Color;

    //Texture *Texture;
};

/**
 * @brief Struct containing mesh information.
 */
struct Mesh
{
    /**
     * @brief Vector of vertices of the mesh.
     */ 
    std::vector<Vertex> Vertices = {};
    
    /**
     * @brief Vector of indices of the mesh.
     */ 
    std::vector<uint16_t> Indices = {};
    //TODO: Remove dis as well?
    /**
     * @brief The material of the mesh.
     */ 
    Material Material;
};

/**
 * @brief Struct containing transformation information of objects in a scene.
 */ 
struct Transform
{
    /**
     * @brief XYZ position.
     */ 
    glm::vec3 Pos = glm::vec3(0);
    
    /**
     * @brief XYZ rotation.
     */ 
    glm::vec3 Rot = glm::vec3(0);

    /**
     * @brief XYZ scaling.
     */ 
    glm::vec3 Scale = glm::vec3(1);

    /**
     * @brief Default constructor of the transform.
     */ 
    Transform() : Transform(glm::vec3(0), glm::vec3(1), glm::vec3(0))
    {
    }

    /**
     * @brief Calculates the rotation matrix from the rotation of the transform.
     * @return Returns a 4x4 rotation matrix.
     */ 
    glm::mat4 Transform::RotationMatrix()
    {
        glm::mat4 rotZ = glm::eulerAngleZ(glm::radians(Rot.z));
        glm::mat4 rotY = glm::eulerAngleY(glm::radians(Rot.y));
        glm::mat4 rotX = glm::eulerAngleX(glm::radians(Rot.x));
        return rotZ * rotY * rotX;
    }

    /**
     * @brief Calculates the translation matrix from the position of the transform.
     * @return Returns a 4x4 translation matrix.
     */
    glm::mat4 Transform::TranslationMatrix()
    {
        return glm::translate(Pos);
    }

    /**
     * @brief Calculates the scale matrix from the scale of the transform.
     * @return Returns a 4x4 scale matrix.
     */
    glm::mat4 Transform::ScaleMatrix()
    {
        return glm::scale(Scale);
    }

    /**
     * @brief Calculates the model matrix of the transform.
     * @return Returns a 4x4 model matrix.
     */
    glm::mat4 Transform::ModelMatrix()
    {
        return TranslationMatrix() * RotationMatrix() * ScaleMatrix();
    }

    glm::vec3 Transform::DirectionVector()
    {
        return glm::normalize(glm::vec3(ModelMatrix()*glm::vec4(0,0,1,0)));
    }

    /**
     * @brief Constructor for the transform, taking all transform information.
     * @param pos The position of the transform
     * @param scale The scale of the transform
     * @param rot The rotation of the transform
     */
    Transform(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot)
    {
        Pos = pos;
        Scale = scale;
        Rot = rot;
    }

    /**
     * @brief Creates a new transform by combining this and other transform.
     * @param other The other transform to be combined with.
     * @return Returns a new Transform.
     */
    Transform operator+(const Transform &other)
    {
        Transform t;
        t.Pos = this->Pos + other.Pos;
        t.Scale = this->Scale * other.Scale;
        t.Rot = this->Rot + other.Rot;
        return t;
    }
};

}; // namespace RTE::Rendering
