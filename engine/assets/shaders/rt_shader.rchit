#version 460
#extension GL_NV_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) rayPayloadInNV vec3 hitValue;
layout(location = 1) hitAttributeNV vec3 attribs;

layout(binding = 3) uniform usamplerBuffer InstanceMapping;
layout(binding = 4) uniform usamplerBuffer IndexBuffers[];



void main()
{
    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    hitValue = barycentrics;

    uint meshIndex = texelFetch(InstanceMapping, gl_InstanceCustomIndexNV).r;
    uvec3 indices = texelFetch(IndexBuffers[nonuniformEXT(meshIndex)], gl_PrimitiveID).rgb;

    /*
    if(meshIndex == 0)
    {
        hitValue = vec3(1.0f,0.0f,0.0f);
    }
    else if (meshIndex == 1)
    {
        hitValue = vec3(0.0,1.0f,0.0f);
    }
    else if (meshIndex == 2)
    {
        hitValue = vec3(0.0,0.0f,1.0f);
    }
    */
    hitValue = ind1;
}