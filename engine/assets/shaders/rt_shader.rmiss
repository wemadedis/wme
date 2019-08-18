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

struct CameraData
{
    float FoV;
    float NearPlane;
    float FarPlane;
    vec4 Position;
    vec4 ClearColor;
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

struct LightData
{
    PointLight PointLights[MAX_LIGHTS];
    uint PointLightCount;
    DirectionalLight DirectionalLights[MAX_LIGHTS];
    uint DirectionalLightCount;
};


layout(binding = 2) uniform WorldData
{
    CameraData Camera;
    LightData Lights;
} World;

layout(location = 0) rayPayloadInNV HitInfo hitValue;

void main()
{
    hitValue.Missed = true;
    hitValue.Color = World.Camera.ClearColor;
    hitValue.Reflectivity = 0.0f;
}