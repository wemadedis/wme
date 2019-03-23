#version 450
#extension GL_ARB_separate_shader_objects : enable

struct DirectionalLight
{
	vec4 Color;
	vec3 Direction;
};

struct PointLight
{
    vec3 Position;
	vec4 Color;
	vec3 Radius;
};

layout(binding = 0) uniform MeshUniformData {
    mat4 ModelMatrix;
    bool HasTexture;
} MeshUniform;

layout(binding = 1) uniform GlobalUniformData
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
    vec4 AmbientColour;
    DirectionalLight DirLight[10];
    PointLight PointLight[10];
} GlobalUniform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec3 fragColor;

layout(location = 1) out vec3 N;
layout(location = 2) out vec3 L;
layout(location = 3) out vec3 V;
layout(location = 4) out vec3 R;


layout(location = 5) out vec2 UV;

layout(location = 6) out int HasTexture;

vec3 lightDir = vec3(0.0f, -1.0f, 1.0f);

//https://learnopengl.com/Lighting/Multiple-lights
void ComputePhong(){
    lightDir = normalize(lightDir);
    L = -normalize(GlobalUniform.ViewMatrix * vec4(GlobalUniform.DirLight[0].Direction,0.0f)).xyz;
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