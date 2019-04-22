#version 460
#extension GL_NV_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : require
layout(location = 0) rayPayloadInNV vec3 hitValue;
layout(location = 1) hitAttributeNV vec3 attribs;


layout(binding = 3) uniform samplerBuffer vertexBuffers[];
layout(binding = 4) uniform samplerBuffer instanceBuffer;
layout(binding = 5) uniform samplerBuffer indexBuffers[];

void main()
{
    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    hitValue = barycentrics;
    int meshIndex = ivec3(texelFetch(instanceBuffer, gl_InstanceCustomIndexNV+1).rgb).r;
    ivec3 indices = ivec3(texelFetch(indexBuffers[meshIndex], gl_PrimitiveID).rgb);
    int v1px = indices.x * 12+3+12+12;
    int v1py = indices.y * 12+4+12+12;
    int v1pz = indices.z * 12+5+12+12;
    vec3 col = vec3(texelFetch(vertexBuffers[meshIndex], v1px).r, texelFetch(vertexBuffers[meshIndex], v1py).r, texelFetch(vertexBuffers[meshIndex], v1pz).r);
    hitValue = col;
    //hitValue = vec3(gl_InstanceCustomIndexNV, 0, 0);
    /*
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec3 normal;
    glm::vec2 texCoord;
    */

    //uint instind = gl_InstanceCustomIndexNV;
    //uint ind22 = InstanceUniform.MeshIndex[instind].index;
    //hitValue = Data[ind22].Vertices[0].pos;
    //hitValue = vec3(0.0f);
}