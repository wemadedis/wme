#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 10

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

struct TransformData
{
    mat4 ModelMatrix;
    mat4 NormalMatrix;
    mat4 MVPMatrix;
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

struct WorldData
{
    PointLight PointLights[MAX_LIGHTS];
    uint PointLightCount;
    DirectionalLight DirectionalLights[MAX_LIGHTS];
    uint DirectionalLightCount;
};

layout(binding = 0) uniform InstanceData {
    TransformData Transform;
    SurfaceData Surface;
} Instance;

layout(binding = 1) uniform GlobalUniformData
{
    CameraData Camera;
    WorldData World;
    //vec4 LightCounts;
    //PointLight PointLights[MAX_LIGHTS];
    //DirectionalLight DirectionalLights[MAX_LIGHTS];
} GlobalUniform;

layout(binding = 2) uniform sampler2D texSampler;


layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 N;
layout(location = 2) in vec3 V;
layout(location = 4) in vec2 UV;
layout(location = 5) in flat int HasTexture;
layout(location = 6) in vec3 PositionCameraSpace;


layout(location = 0) out vec4 outColor;

vec4 Phong(vec3 L, vec3 R)
{
    float diff = max(0.0f, dot(L,N)) * Instance.Surface.Diffuse;
    float spec = pow(max(0.0f, dot(-V,R)) * Instance.Surface.Specular, Instance.Surface.Shininess);
    return vec4(diff) + vec4(spec);
}

vec4 CalculatePointLightShading(PointLight light)
{
    vec3 lightPosition = vec3(GlobalUniform.Camera.ViewMatrix * vec4(light.PositionRadius.xyz,1.0f));
    vec3 direction = lightPosition - PositionCameraSpace;
    vec3 L = normalize(direction);
    vec3 R = reflect(-L,N);
    float distance = length(direction);
    return Phong(L,R) * light.Color * light.PositionRadius.w / (distance*distance);
}

vec4 CalculateDirectionalLightShading(DirectionalLight light)
{
    vec3 L = normalize(vec3(GlobalUniform.Camera.ViewMatrix * vec4(light.Direction.xyz,0.0f)));
    vec3 R = normalize(reflect(L, N));
    return Phong(L,R) * light.Color;
}

vec4 CalculatePerLightShading()
{
    vec4 color = Instance.Surface.Color*Instance.Surface.Ambient;
    if(HasTexture != 0)
    {
        color = texture(texSampler, UV)*Instance.Surface.Ambient;
    }
    for(uint pointLightIndex = 0; pointLightIndex < GlobalUniform.World.PointLightCount; pointLightIndex++)
    {
        color += CalculatePointLightShading(GlobalUniform.World.PointLights[pointLightIndex]);
    }
    for(uint directionalLightIndex = 0; directionalLightIndex < GlobalUniform.World.DirectionalLightCount; directionalLightIndex++)
    {
        color += CalculateDirectionalLightShading(GlobalUniform.World.DirectionalLights[directionalLightIndex]);
    }
    return color;
}

void main() 
{
    outColor =  CalculatePerLightShading();
}