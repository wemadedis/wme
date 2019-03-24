#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 2) in vec3 L;
layout(location = 1) in vec3 N;
layout(location = 3) in vec3 V;
layout(location = 4) in vec3 R;
layout(location = 5) in vec2 UV;
layout(location = 6) in flat int HasTexture;
layout(location = 7) in float Distance;
layout(location = 8) in vec4 Color;
layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;

struct DirectionalLight
{
	vec4 Color;
	vec3 Direction;
};

struct PointLight
{
    vec4 Color;
    vec3 Position;
	float Radius;
};

const uint MAX_LIGHTS = 10;

layout(binding = 1) uniform GlobalUniformData
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
    vec4 AmbientColor;
    PointLight PointLights[MAX_LIGHTS];
    DirectionalLight DirectionalLights[MAX_LIGHTS];
    uint PointLightCount;
    uint DirectionalLightCount;
} GlobalUniform;

vec4 ambient = vec4(0.1);

void Phong()
{
    float diff = max(0.0f, dot(L,N));
    float spec = max(0.0f, dot(V,R));
    vec4 texSample = texture(texSampler, UV);
    outColor = texSample * ambient + texSample * GlobalUniform.PointLights[0].Radius / (Distance * Distance);
}

void CalculatePointLights()
{
    
}

void main() 
{
    if(HasTexture != 0)
    {
        Phong();    
    } 
    else
    {
        outColor = vec4(0.0);
    }
    
    
}