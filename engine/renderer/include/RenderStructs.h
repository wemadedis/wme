#pragma once
#include <glm\glm.hpp>

namespace RTE_Renderer
{
	/// <summary>Vertex struct for use in RTE rendering.</summary>
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec4 Color;
	};

	struct Camera
	{
		glm::vec3 Position;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
	};

	enum class LightType {DIRECTIONAL, POINT, AMBIENT};

	struct Light
	{
		LightType LightType;
		glm::vec4 Color;
		union
		{
			struct
			{
				glm::vec3 Direction;
			};
			struct
			{
				float Radius;
				float Intensity;
			};
		};
	};

	struct Texture
	{

	};

	struct Shader
	{
	};

	struct Material
	{
		Texture* _Texture;
	};

	struct Mesh
	{
		int _VertexCount;
		Vertex* _Vertices;
		Material _Material;
	};
};