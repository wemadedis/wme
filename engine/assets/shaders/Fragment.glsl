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
layout(set = 0, binding = 2) uniform sampler2D Texture;
layout(location = 4) in vec3 out_normal;
layout(location = 5) in vec3 out_eye;
layout(location = 6) in vec2 out_UV;
layout(location = 7) in vec3 out_position_viewspace;
vec4 Phong(vec3 L, vec3 R)
{
    float diff = max(0.0f, dot(L,N)) * Instance.Surface.Diffuse;
    float spec = pow(max(0.0f, dot(-V,R)) * Instance.Surface.Specular, Instance.Surface.Shininess);
    return vec4(diff) + vec4(spec)
}

vec4 CalculatePointLightShading(PointLight light)
{
    vec3 lightPosition = vec3(World.Camera.ViewMatrix * vec4(light.PositionRadius.xyz,1.0f));
    vec3 direction = lightPosition - PositionCameraSpace;
    vec3 L = normalize(direction);
    vec3 R = reflect(-L,N);
    float distance = length(direction);
    return Phong(L,R) * light.Color * light.PositionRadius.w / (distance*distance);
}

vec4 CalculateDirectionalLightShading(DirectionalLight light)
{
    vec3 L = normalize(vec3(World.Camera.ViewMatrix * vec4(light.Direction.xyz,0.0f)));
    vec3 R = normalize(reflect(L, N));
    return Phong(L,R) * light.Color;
}

void main() 
{
    outColor =  CalculatePerLightShading();
}

