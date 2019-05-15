#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "DeviceMemoryManager.h"
#include "rte/RendererHandles.h"
#include "rte/RenderStructs.h"

namespace RTE::Rendering
{


/**
 * @brief An enum containing the varioues shader types used by the renderer.
 */
enum class ShaderType
{
    /**
     * @brief Vertex shader type.
     */
    VERTEX,

    /**
     * @brief Fragment shader type.
     */
    FRAGMENT,

    /**
     * @brief Ray generation shader type.
     */
    RAYGEN,

    /**
     * @brief Ray closest hit shader type.
     */
    RCHIT,

    /**
     * @brief Ray miss shader type.
     */
    RMISS
};

/**
 * @brief Struct containing shader information for rendering.
 */
struct ShaderInfo
{
	/**
     * @brief The type of the shader.
     */
	ShaderType Type;

	/**
     * @brief The Vulkan handle to a shader module.
     */
	VkShaderModule Module;
};

/**
 * @brief Struct containing image information for rendering.
 */ 
struct ImageInfo
{
	/**
	 * @brief Image memory allocation information.
	 */
    ImageInformation imageInfo;

	/**
	 * @brief The Vulkan image view handle.
	 */
    VkImageView imageView;
};

/**
 * @brief Struct containing texture information for rendering.
 */
struct TextureInfo
{
	/**
	 * @brief Vulkan sampler specifying how to sample the texture.
	 */
	VkSampler sampler;
	/**
	 * @brief Information about the texture image.
	 */
	ImageInfo image;
};

/**
 * @brief Struct containing information of a mesh instance.
 */
struct MeshInstanceUniformData
{
	/**
	 * @brief A 4x4 model matrix.
	 */
	glm::mat4 ModelMatrix;

	/**
	 * @brief A 4x4 normal matrix used on normals to avoid issues with non-uniform scaling.
	 */
	glm::mat4 NormalMatrix;

	/**
	 * @brief The ambient reflection instensity.
	 */
	float Ambient = 0.2f;
    
	/**
	 * @brief The diffuse reflection instensity.
	 */
	float Diffuse = 0.3f;
    
	/**
	 * @brief The specular reflection instensity.
	 */
	float Specular = 1.0f;
    
	/**
	 * @brief The shininess coefficient.
	 */
	float Shininess = 10.0f;

	/**
	 * @brief Reflectivity of the surface. Used for ray tracing.
	 */
    float Reflectivity = 0.0f;

	/**
	 * @brief Transparency of the surface. Used for ray tracing.
	 */
    float Transparency = 0.0f;
	int padding[2];
	/**
	 * @brief The color of the mesh.
	 */
	glm::vec4 Color = glm::vec4(1.0f);

	/**
	 * @brief Handle to the texture bound to this instance.
	 */
	TextureHandle Texture;
	/**
	 * @brief Boolean indicating if the instance has a texture bound to it.
	 */
	bool HasTexture;
};

const uint32_t MAX_LIGHTS = 10;

/**
 * @brief Struct encapsulating the directional light information.
 */
struct DirectionalLight
{
	/**
	 * @brief The RGBA color of the light.
	 */
	glm::vec4 Color;

	/**
	 * @brief The direction of the light.
	 */
	glm::vec4 Direction;
};

/**
 * @brief Struct encapsulating the point light information.
 */
struct PointLight
{
	/**
	 * @brief The RGBA color of the light.
	 */
	glm::vec4 Color;

	/**
	 * @brief Vector containing the position in XYZ, and the light radius as the W component.
	 */
    glm::vec4 PositionRadius;
};

/**
 * @brief Struct containing the global data for rendering.
 */
struct GlobalUniformData
{
	/**
	 * @brief Field of view given in degrees.
	 */
	float FieldOfView;

	/**
	 * @brief Distance to the near plane.
	 */
	float NearPlane;
	
	/**
	 * @brief Distance to the far plane.
	 */
	float FarPlane;

	//Padding to meet memory alignment requirement.
	int padding;

	/**
	 * @brief The position of the camera.
	 */
	glm::vec4 Position;

	/**
	 * @brief The camera view matrix.
	 */
	glm::mat4 ViewMatrix;

	/**
	 * @brief The camera projection matrix.
	 */
	glm::mat4 ProjectionMatrix;

	/**
	 * @brief The clear color used before rendering a frame.
	 */
    glm::vec4 ClearColor;

	/**
	 * @brief 4D vector containing the number of directional lights as the X value, and number of point lights as the Y value. Remaining Y & W components are ignored.
	 */
	glm::vec4 LightCounts;

	/**
	 * @brief Array of point lights of size MAX_LIGHTS
	 */
	PointLight PointLights[MAX_LIGHTS] = {};
	
	/**
	 * @brief Array of directional lights of size MAX_LIGHTS
	 */
    DirectionalLight DirectionalLights[MAX_LIGHTS] = {};
};

/**
 * @brief Struct encapsulating information of a mesh.
 */
struct MeshInfo
{
	/**
	 * @brief The number of vertices defining the mesh.
	 */
	uint64_t VertexCount;

	/**
	 * @brief The number of indices defining the triangles of the mesh.
	 */
    uint64_t IndexCount;

	/**
	 * @brief Vertex buffer memory allocation information.
	 */
	BufferInformation vertexBuffer  = {};

	/**
	 * @brief Index buffer memory allocation information.
	 */
	BufferInformation indexBuffer = {};
};

/**
 * @brief Struct encapsulating mesh instance information.
 */
struct MeshInstance
{
	/**
	 * @brief Handle to the mesh the instance represents.
	 */
	MeshHandle mesh;

	/**
	 * @brief Handle to the texture bound to the instance.
	 */
	TextureHandle texture;

	/**
	 * @brief The instance uniform buffer memory allocation information.
	 */
	BufferInformation uniformBuffer = {};
};

/**
 * @brief Struct encapsulating mesh instance information for creation of the top level acceleration structure used for ray tracing.
 */
struct VkGeometryInstance
{
	/**
	 * @brief A 4x3 matrix containg the transformation information of the mesh instance (row-major order).
	 */
    float transform[12];
    uint32_t instanceId : 24;
    uint32_t mask : 8;
    uint32_t instanceOffset : 24;
    uint32_t flags : 8;
    uint64_t accelerationStructureHandle;
};

};