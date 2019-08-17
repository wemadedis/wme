#version 460
#extension GL_NV_ray_tracing : require

#define MAX_LIGHTS 10

struct HitInfo
{
    bool Missed;
    vec3 Point;
    vec3 Normal;
    vec2 UV;
    vec4 Color;
    float Reflectivity;
};

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

layout(binding = 2) uniform GlobalUniformData
{
    float FieldOfView;
    float NearPlane;
    float FarPlane;
    vec4 Position;
    vec4 ClearColor;
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
    PointLight PointLights[MAX_LIGHTS];
    uint PointLightCount;
    DirectionalLight DirectionalLights[MAX_LIGHTS];
    uint DirectionalLightCount;
} GlobalUniform;

layout(location = 0) rayPayloadInNV HitInfo hitValue;

void main()
{
    hitValue.Missed = true;
    hitValue.Color = GlobalUniform.ClearColor;
    hitValue.Reflectivity = 0.0f;
}