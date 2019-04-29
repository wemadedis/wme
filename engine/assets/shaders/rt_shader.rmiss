#version 460
#extension GL_NV_ray_tracing : require

struct HitInfo
{
    bool Missed;
    vec3 Point;
    vec3 Normal;
    vec2 UV;
    vec4 Color;
    float Reflectivity;
};

layout(location = 0) rayPayloadInNV HitInfo hitValue;

void main()
{
    hitValue.Missed = true;
    hitValue.Color = vec4(0.0f);// vec3(0.0, 0.1, 0.3);
}