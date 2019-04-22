#version 460
#extension GL_NV_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) rayPayloadInNV vec3 hitValue;
layout(location = 1) hitAttributeNV vec3 attribs;


layout(binding = 3) uniform usamplerBuffer InstanceMapping;
layout(set = 0, binding = 4) uniform usamplerBuffer IndexBuffers[];
layout(set = 1, binding = 4) uniform samplerBuffer VertexBuffers[];

struct Vertex
{
    vec3 pos;
    vec4 color;
    vec3 normal;
    vec2 texCoord;
};

float FetchFloat(uint meshIndex, int floatPlacement)
{
    return texelFetch(VertexBuffers[nonuniformEXT(meshIndex)], floatPlacement).r;
}

Vertex GetVertex(uint meshIndex, int vertexIndex)
{
    Vertex vert;
    vert.pos.x = FetchFloat(meshIndex,  vertexIndex * 12);
    vert.pos.x = FetchFloat(meshIndex,  vertexIndex * 12+1);
    vert.pos.x = FetchFloat(meshIndex,  vertexIndex * 12+2);
    vert.color.r = FetchFloat(meshIndex,  vertexIndex * 12+3);
    vert.color.g = FetchFloat(meshIndex,  vertexIndex * 12+4);
    vert.color.b = FetchFloat(meshIndex,  vertexIndex * 12+5);
    vert.color.a = FetchFloat(meshIndex,  vertexIndex * 12+6);
    vert.normal.x = FetchFloat(meshIndex,  vertexIndex * 12+7);
    vert.normal.y = FetchFloat(meshIndex,  vertexIndex * 12+8);
    vert.normal.z = FetchFloat(meshIndex,  vertexIndex * 12+9);
    vert.texCoord.x = FetchFloat(meshIndex,  vertexIndex * 12+10);
    vert.texCoord.y = FetchFloat(meshIndex,  vertexIndex * 12+11);
    return vert;
}

void main()
{
    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    hitValue = barycentrics;

    uint meshIndex = texelFetch(InstanceMapping, gl_InstanceCustomIndexNV).r;
    uvec3 indices = texelFetch(IndexBuffers[nonuniformEXT(meshIndex)], gl_PrimitiveID).rgb;
    Vertex v1 = GetVertex(meshIndex, int(indices.x));
    Vertex v2 = GetVertex(meshIndex, int(indices.y));
    Vertex v3 = GetVertex(meshIndex, int(indices.z));
    //vec3 position = 
    vec3 col = v1.color.rgb * barycentrics.x + v2.color.rgb * barycentrics.y + v3.color.rgb * barycentrics.z;
    hitValue = col;

}