#version 460
#extension GL_NV_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : require


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

struct Vertex
{
    vec3 pos;
    vec4 color;
    vec3 normal;
    vec2 texCoord;
};

struct HitInfo
{
    vec3 Point;
    vec3 Normal;
    vec2 UV;
};

layout(location = 0) rayPayloadInNV vec3 hitValue;
layout(location = 1) hitAttributeNV vec3 attribs;

layout(binding = 2) uniform GlobalUniformData
{
    vec4 Position;
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
    vec4 AmbientColor;
    vec4 LightCounts;
    PointLight PointLights[MAX_LIGHTS];
    DirectionalLight DirectionalLights[MAX_LIGHTS];
} GlobalUniform;

layout(binding = 3) uniform usamplerBuffer InstanceMapping;
layout(set = 0, binding = 4) uniform usamplerBuffer IndexBuffers[];
layout(set = 1, binding = 4) uniform samplerBuffer VertexBuffers[];
layout(set = 2, binding = 4) uniform InstanceUniformData
{
    mat4 ModelMatrix;
    vec4 Color;
    bool HasTexture;
} InstanceData[];

float FetchFloat(uint meshIndex, int floatPlacement)
{
    return texelFetch(VertexBuffers[nonuniformEXT(meshIndex)], floatPlacement).r;
}


#define VERTEX_SIZE_FLOATS 12

Vertex GetVertex(uint meshIndex, int vertexIndex)
{
    Vertex vert;
    vert.pos.x =    FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS    );
    vert.pos.x =    FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 1);
    vert.pos.x =    FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 2);
    vert.color.r =  FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 3);
    vert.color.g =  FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 4);
    vert.color.b =  FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 5);
    vert.color.a =  FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 6);
    vert.normal.x = FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 7);
    vert.normal.y = FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 8);
    vert.normal.z = FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 9);
    vert.texCoord.x=FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 10);
    vert.texCoord.y=FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 11);
    return vert;
}

HitInfo GetHitInfo(Vertex v1, Vertex v2, Vertex v3, vec3 barycentrics)
{
    mat4 modelMatrix = InstanceData[nonuniformEXT(gl_InstanceCustomIndexNV)].ModelMatrix;
    vec3 position = v1.pos * barycentrics.x + v2.pos * barycentrics.y + v3.pos * barycentrics.z;
    vec3 normal = normalize(v1.normal * barycentrics.x + v2.normal * barycentrics.y + v3.normal * barycentrics.z);
    vec2 UV = v1.texCoord * barycentrics.x + v2.texCoord * barycentrics.y + v3.texCoord * barycentrics.z;
    
    
    normal =vec3(modelMatrix * vec4(normal,0.0f));
    if(dot(normal, gl_WorldRayDirectionNV) < 0.0f) normal = -normal;
    
    position = vec3(modelMatrix * vec4(position,1.0f));

    HitInfo info;
    info.Point = position;
    info.Normal = normal;
    info.UV = UV;
    return info;
}

vec4 Phong(vec3 L, vec3 R, vec3 N)
{
    float diff = max(0.0f, dot(L,N));
    float spec = max(0.0f, dot(-gl_WorldRayDirectionNV,R));
    return vec4(1.0f) * diff * spec;
}

vec4 CalculatePointLightShading(PointLight light, HitInfo hitinfo)
{
    vec3 lightPosition = light.PositionRadius.xyz;
    vec3 direction =  lightPosition - hitinfo.Point;
    vec3 L = -normalize(direction);
    vec3 R = reflect(L,hitinfo.Normal);
    float distance = length(direction);
    return Phong(L,R, hitinfo.Normal) * light.Color * light.PositionRadius.w / (distance*distance);
}
/*
vec4 CalculateDirectionalLightShading(DirectionalLight light, vec3 N)
{
    vec3 L = light.Direction.xyz;
    vec3 R = reflect(L, N);
    return Phong(L,R) * light.Color;
}
*/



vec4 CalculatePerLightShading(HitInfo hitinfo)
{
    vec4 color = GlobalUniform.AmbientColor;
    for(uint pointLightIndex = 0; pointLightIndex < GlobalUniform.LightCounts.y; pointLightIndex++)
    {
        color += CalculatePointLightShading(GlobalUniform.PointLights[pointLightIndex], hitinfo);
    }
    /*for(uint directionalLightIndex = 0; directionalLightIndex < GlobalUniform.LightCounts.x; directionalLightIndex++)
    {
        color += CalculateDirectionalLightShading(GlobalUniform.DirectionalLights[directionalLightIndex]);
    }*/
    return color;
}

void main()
{
    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    uint meshIndex = texelFetch(InstanceMapping, gl_InstanceCustomIndexNV).r;
    int index1 = int(texelFetch(IndexBuffers[nonuniformEXT(meshIndex)], gl_PrimitiveID*3).r);
    int index2 = int(texelFetch(IndexBuffers[nonuniformEXT(meshIndex)], gl_PrimitiveID*3+1).r);
    int index3 = int(texelFetch(IndexBuffers[nonuniformEXT(meshIndex)], gl_PrimitiveID*3+2).r);
    Vertex v1 = GetVertex(meshIndex, index1);
    Vertex v2 = GetVertex(meshIndex, index2);
    Vertex v3 = GetVertex(meshIndex, index3);
    HitInfo hitinfo = GetHitInfo(v1, v2, v3, barycentrics);
    hitValue = CalculatePerLightShading(hitinfo).rgb;
}