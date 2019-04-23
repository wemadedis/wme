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

vec4 Phong(vec3 L, vec3 R, vec3 N)
{
    float diff = max(0.0f, dot(L,N));
    float spec = max(0.0f, dot(-gl_WorldRayDirectionNV,R));
    return vec4(0.5f) * diff;
}

vec4 CalculatePointLightShading(PointLight light, vec3 hitPosition, vec3 hitNormal)
{
    vec3 lightPosition = light.PositionRadius.xyz;
    vec3 direction = lightPosition - hitPosition;
    vec3 L = normalize(direction);
    vec3 R = reflect(L,hitNormal);
    float distance = length(direction);
    //if(distance > light.Radius) return Phong(L,N);
    return Phong(L,R, hitNormal) * light.Color * light.PositionRadius.w / (distance*distance);
}
/*
vec4 CalculateDirectionalLightShading(DirectionalLight light, vec3 N)
{
    vec3 L = light.Direction.xyz;
    vec3 R = reflect(L, N);
    return Phong(L,R) * light.Color;
}
*/
vec4 CalculatePerLightShading(vec3 hitPosition, vec3 hitNormal)
{
    vec4 color = vec4(0.5f) * GlobalUniform.AmbientColor;
    for(uint pointLightIndex = 0; pointLightIndex < GlobalUniform.LightCounts.y; pointLightIndex++)
    {
        color += CalculatePointLightShading(GlobalUniform.PointLights[pointLightIndex], hitPosition, hitNormal);
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
    ivec3 indices = ivec3(texelFetch(IndexBuffers[nonuniformEXT(meshIndex)], gl_PrimitiveID).rgb);
    Vertex v1 = GetVertex(meshIndex, indices.x);
    Vertex v2 = GetVertex(meshIndex, indices.y);
    Vertex v3 = GetVertex(meshIndex, indices.z);
    vec3 position = v1.pos * barycentrics.x + v2.pos * barycentrics.y + v3.pos * barycentrics.z;
    vec3 normal = normalize(v1.normal * barycentrics.x + v2.normal * barycentrics.y + v3.normal * barycentrics.z);
    normal = mat3(gl_ObjectToWorldNV ) * normal;

    //if(dot(normal, gl_WorldRayDirectionNV) < 0.0f) normal = -normal;
    hitValue = normal;
    hitValue = CalculatePerLightShading(position, normal).rgb;
    //vec3 col = v1.color.rgb * barycentrics.x + v2.color.rgb * barycentrics.y + v3.color.rgb * barycentrics.z;
    //hitValue = col;

}