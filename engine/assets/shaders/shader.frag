#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 2) in vec3 L;
layout(location = 1) in vec3 N;
layout(location = 3) in vec3 V;
layout(location = 4) in vec3 R;
layout(location = 5) in vec2 UV;
layout(location = 6) in flat int HasTexture;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;

vec4 ambient = vec4(0.1);

void Phong(){
    float diff = max(0.0f, dot(L,N));
    float spec = max(0.0f, dot(V,R));
    vec4 texSample = texture(texSampler, UV);
    outColor = texSample * ambient + texSample * diff;
}

void main() {
    if(HasTexture != 0)
    {
        Phong();    
    } 
    else
    {
        outColor = vec4(fragColor, 1.0);
    }
    
    
}