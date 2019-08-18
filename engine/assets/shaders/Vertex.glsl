#version 450
#extension GL_ARB_separate_shader_objects : enable
#define MAX_LIGHTS 10


struct DirectionalLight
{
    vec4 Color;
    vec4 Direction;
};

struct TransformData
{
    mat4 ModelMatrix;
    mat4 NormalMatrix;
    mat4 MVPMatrix;
};

struct PointLight
{
    vec4 Color;
    vec4 PositionRadius;
};

struct SurfaceData
{
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
    float Reflectivity;
    float Transparency;
    vec4 Color;
    uint Texture;
    bool HasTexture;
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
    PointLight PointLights[10];
    uint PointLightCount;
    DirectionalLight DirectionalLights[10];
    uint DirectionalLightCount;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_UV;
layout(set = 0, binding = 0) uniform InstanceData
{
    TransformData Transform;
    SurfaceData Surface;
} Instance;
layout(set = 0, binding = 1) uniform WorldData
{
    CameraData Camera;
    LightData Lights;
} World;
layout(location = 4) out vec3 out_normal;
layout(location = 5) out vec3 out_eye;
layout(location = 6) out vec2 out_UV;
layout(location = 7) out vec3 out_position_viewspace;
void CalculatePhongComponents()
{
    mat4 modelView = Camera.ViewMatrix * Transform.ModelMatrix;
    vec4 position = vec4(in_position,1.0f)
    out_position_viewspace = vec3(modelView*position);
    vec4 normal = Transform.NormalMatrix * vec4(in_normal);
    out_normal = normalize(vec3(normal));
    out_eye = normalize(out_position_viewspace);
    if(dot(out_normal, out_eye) > 0) out_normal = -out_normal;
}

void main()
{
    gl_Position = Transform.MVPMatrix * vec4(in_position,1.0f);
    CalculatePhongComponents();
    out_UV = in_UV;
}

