#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec3 fragColor;

vec3 lightDir = vec3(0.0f, -1.0f, 0.0f);


void ComputePhong(){
    vec3 globNormal = vec3(0.0f, 1.0f, 0.0f); //(ubo.proj * ubo.view * ubo.model * vec4(normal, 0.0f)).xyz;
    vec3 invLight = -lightDir;
    float diff = dot(globNormal, invLight);
    vec3 reflection = reflect(invLight, globNormal);
    vec3 eye = -normalize(gl_Position.xyz);
    float spec = dot(globNormal, eye);
    fragColor = diff * spec * inColor;
}


void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    ComputePhong();
    //fragColor = inColor;
}