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

layout(binding = 0) uniform InstanceUniformData {
    mat4 ModelMatrix;
    mat4 NormalMatrix;
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
    float Reflectivity;
    float Transparency;
    uint Texture;
    bool HasTexture;
} InstanceUniform;

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
    float diff = max(0.0f, dot(L,N)) * InstanceUniform.Diffuse;
    float spec = pow(max(0.0f, dot(-V,R)) * InstanceUniform.Specular, InstanceUniform.Shininess);
    return vec4(diff) + vec4(spec);
}

vec4 CalculatePointLightShading(PointLight light)
{
    vec3 lightPosition = vec3(GlobalUniform.ViewMatrix * vec4(light.PositionRadius.xyz,1.0f));
    vec3 direction = lightPosition - PositionCameraSpace;
    vec3 L = normalize(direction);
    vec3 R = reflect(-L,N);
    float distance = length(direction);
    return Phong(L,R) * light.Color * light.PositionRadius.w / (distance*distance);
}

vec4 CalculateDirectionalLightShading(DirectionalLight light)
{
    vec3 L = normalize(vec3(GlobalUniform.ViewMatrix * vec4(light.Direction.xyz,0.0f)));
    vec3 R = normalize(reflect(L, N));
    return Phong(L,R) * light.Color;
}

vec4 CalculatePerLightShading()
{
    vec4 color = vec4(InstanceUniform.Ambient);
    if(HasTexture != 0)
    {
        color *= texture(texSampler, UV);
    }
    for(uint pointLightIndex = 0; pointLightIndex < GlobalUniform.LightCounts.y; pointLightIndex++)
    {
        color += CalculatePointLightShading(GlobalUniform.PointLights[pointLightIndex]);
    }
    for(uint directionalLightIndex = 0; directionalLightIndex < GlobalUniform.LightCounts.x; directionalLightIndex++)
    {
        color += CalculateDirectionalLightShading(GlobalUniform.DirectionalLights[directionalLightIndex]);
    }
    return color;
}

void main() 
{
    outColor =  CalculatePerLightShading();
}