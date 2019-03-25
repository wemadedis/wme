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

layout(binding = 1) uniform GlobalUniformData
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
    vec4 AmbientColor;
    vec4 LightCounts;
    PointLight PointLights[MAX_LIGHTS];
    DirectionalLight DirectionalLights[MAX_LIGHTS];
} GlobalUniform;

layout(binding = 2) uniform sampler2D texSampler;


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 N;
layout(location = 2) in vec3 V;
layout(location = 4) in vec2 UV;
layout(location = 5) in flat int HasTexture;
layout(location = 6) in vec3 PositionCameraSpace;


layout(location = 0) out vec4 outColor;

vec4 Phong(vec3 L, vec3 R)
{
    float diff = max(0.0f, dot(L,N));
    float spec = max(0.0f, dot(V,R));
    return texture(texSampler, UV) * diff *spec;
}

vec4 CalculatePointLightShading(PointLight light)
{
    vec4 lightCameraSpace = GlobalUniform.ViewMatrix * vec4(light.PositionRadius.xyz,1.0f);
    vec3 lightPosition = vec3(lightCameraSpace);
    vec3 direction = PositionCameraSpace - lightPosition;
    vec3 L = normalize(direction);
    vec3 R = reflect(L,N);
    float distance = length(direction);
    //if(distance > light.Radius) return Phong(L,N);
    return Phong(L,R) * light.Color * light.PositionRadius.w / (distance*distance);
}

vec4 CalculateDirectionalLightShading(DirectionalLight light)
{
    vec3 L = vec3(GlobalUniform.ViewMatrix * vec4(light.Direction.xyz,0.0f));
    vec3 R = reflect(L, N);
    return Phong(L,R) * light.Color;
}

vec4 CalculatePerLightShading()
{
    vec4 color = texture(texSampler, UV) * GlobalUniform.AmbientColor;
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
    if(HasTexture != 0)
    {
        outColor = CalculatePerLightShading();   
    } 
    else
    {
        outColor = vec4(0.0);
    }
    
    
}