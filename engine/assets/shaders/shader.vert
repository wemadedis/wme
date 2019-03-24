#version 450
#extension GL_ARB_separate_shader_objects : enable

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

layout(binding = 0) uniform MeshUniformData {
    mat4 ModelMatrix;
    bool HasTexture;
} MeshUniform;

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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

layout(location = 1) out vec3 N;
layout(location = 2) out vec3 L;
layout(location = 3) out vec3 V;
layout(location = 4) out vec3 R;


layout(location = 5) out vec2 UV;

layout(location = 6) out int HasTexture;

layout(location = 7) out float Distance;
layout(location = 8) out vec4 Color;

//https://learnopengl.com/Lighting/Multiple-lights
void ComputePhong(){
    vec4 pos = MeshUniform.ModelMatrix * vec4(inPosition, 1.0);
    vec3 poss = pos.xyz/pos.w;
    vec3 distance = poss - GlobalUniform.PointLights[0].Position;
    Distance = length(distance);
    vec3 lightDir = normalize(distance);
    Color = GlobalUniform.PointLights[0].Color;
    L = normalize(GlobalUniform.ViewMatrix * vec4(lightDir,0.0f)).xyz;
    N = normalize(GlobalUniform.ViewMatrix * MeshUniform.ModelMatrix *  vec4(normal,0.0f)).xyz;
    V = -normalize(GlobalUniform.ViewMatrix * MeshUniform.ModelMatrix * vec4(inPosition, 1.0f)).xyz;
    //Flip the normal if it points away from the eye (REMOVE THIS LATER)
    if(dot(N,V) < 0) N = -N;
    R = reflect(L,N);
}


void main() {
    HasTexture = MeshUniform.HasTexture ? 1 : 0;
    gl_Position = GlobalUniform.ProjectionMatrix * GlobalUniform.ViewMatrix * MeshUniform.ModelMatrix * vec4(inPosition, 1.0);
    ComputePhong();
    fragColor = inColor;
    UV = texCoord;
}