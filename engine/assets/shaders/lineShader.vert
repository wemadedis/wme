#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

struct DirectionalLight
{
	vec4 Color;
	vec4 Direction;
};

struct PointLight
{
    vec4 Color;
    vec4 PositionRadius;
};

#define MAX_LIGHTS 10

layout(binding = 1) uniform GlobalUniformData
{
    float FieldOfView;
    float NearPlane;
    float FarPlane;
    vec4 Position;
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
    vec4 ClearColor;
    vec4 LightCounts;
    PointLight PointLights[MAX_LIGHTS];
    DirectionalLight DirectionalLights[MAX_LIGHTS];
} GlobalUniform;


void main() {
    gl_Position = GlobalUniform.ProjectionMatrix * GlobalUniform.ViewMatrix * vec4(inPosition,1.0f);
    fragColor = vec4(0,1,0,1);
}