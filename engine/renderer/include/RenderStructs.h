#pragma once
#include <glm\glm.hpp>

namespace RTE_Renderer
{
	/// <summary>Vertex struct for use in RTE rendering.</summary>
	struct Vertex
	{
		glm::vec3 _Position;
		glm::vec3 _Normal;
		glm::vec4 _Color;
	};

	struct Camera
	{
		glm::vec3 _Position;
		glm::mat4 _ViewMatrix;
		glm::mat4 _ProjectionMatrix;
	};

	enum class LightType {DIRECTIONAL, POINT, AMBIENT};

	struct Light
	{
		LightType _LightType;
		glm::vec4 _Color;
		union
		{
			struct
			{
				glm::vec3 _Direction;
			} _DirectionalLight;
			struct
			{
				float _Radius;
				float _Intensity;
			} _PointLight;
		} _Properties;
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