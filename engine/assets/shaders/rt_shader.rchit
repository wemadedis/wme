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
    bool Missed;
    vec3 Point;
    vec3 Normal;
    vec2 UV;
    vec4 Color;
    float Reflectivity;
};

layout(location = 0) rayPayloadInNV HitInfo hitValue;
layout(location = 1) hitAttributeNV vec3 attribs;
layout(location = 2) rayPayloadInNV float shadowRayHitValue;


layout(binding = 0) uniform accelerationStructureNV topLevelAS;

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
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
    float Reflectivity;
    float Transparency;
    uint Texture;
    bool HasTexture;
} InstanceData[];

layout(set = 3, binding = 4) uniform sampler2D TextureSamplers[];

float FetchFloat(uint meshIndex, int offset)
{
    return texelFetch(VertexBuffers[nonuniformEXT(meshIndex)], offset).r;
}


#define VERTEX_SIZE_FLOATS 12

Vertex GetVertex(uint meshIndex, int vertexIndex)
{
    Vertex vert;
    vert.pos.x =    FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS    );
    vert.pos.y =    FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 1);
    vert.pos.z =    FetchFloat(meshIndex, vertexIndex * VERTEX_SIZE_FLOATS + 2);
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
    vec3 normal = normalize(v1.normal * barycentrics.x + v2.normal * barycentrics.y + v3.normal * barycentrics.z);
    vec2 UV = v1.texCoord * barycentrics.x + v2.texCoord * barycentrics.y + v3.texCoord * barycentrics.z;
    
    
    normal = normalize(vec3(modelMatrix * vec4(normal,0.0f)));
    if(dot(normal, gl_WorldRayDirectionNV) < 0.0f) normal = -normal;

    hitValue.Missed = false;
    hitValue.Point = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV;//position;
    hitValue.Normal = normal;
    hitValue.UV = UV;
    hitValue.Color = vec4(0.0f);
    hitValue.Reflectivity = InstanceData[nonuniformEXT(gl_InstanceCustomIndexNV)].Reflectivity;
    return hitValue;
}

float FireShadowRay(vec3 origin, vec3 direction)
{
    //origin = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_HitTNV; //TODO: THIS MOTHER@!#$€?
    //vec3 direction = dir;
    uint rayFlags = gl_RayFlagsOpaqueNV | gl_RayFlagsTerminateOnFirstHitNV;
    uint cullMask = 0xff;
    float tmin = 0.001;
    float tmax = 100.0;

    traceNV(topLevelAS, rayFlags, cullMask, 1 /* sbtRecordOffset */, 0 /* sbtRecordStride */, 1 /* missIndex */, origin, tmin, -direction, tmax, 2 /*payload location*/);
    return shadowRayHitValue;
}



vec4 Phong(vec3 L, vec3 R, vec3 N, vec3 O)
{
    if(FireShadowRay(O-N*0.01f, L) < 100.0f) return vec4(0.0f);
    float udiff = InstanceData[gl_InstanceCustomIndexNV].Diffuse;
    float uspec = InstanceData[gl_InstanceCustomIndexNV].Specular;
    float shininess = InstanceData[gl_InstanceCustomIndexNV].Shininess;
    float diff = max(0.0f, dot(L,N))*udiff;
    float spec = max(0.0f, dot(-gl_WorldRayDirectionNV,R))*uspec;
    /*if(InstanceData[gl_InstanceCustomIndexNV].HasTexture)
    {
        uint texIndex = InstanceData[gl_InstanceCustomIndexNV].Texture;
        vec4 texColor = texture(TextureSamplers[texIndex], hitValue.UV);    
        return texColor * diff + texColor * spec;
    }*/
    return vec4(diff) + vec4(spec);
}

vec4 CalculatePointLightShading(PointLight light, HitInfo hitInfo)
{
    vec3 lightPosition = light.PositionRadius.xyz;
    vec3 direction =  lightPosition - hitInfo.Point;
    vec3 L = -normalize(direction);
    vec3 R = reflect(L,hitInfo.Normal);
    float distance = length(direction);
    return Phong(L,R, hitInfo.Normal, hitInfo.Point) * light.Color * light.PositionRadius.w / (distance*distance);
}


vec4 CalculateDirectionalLightShading(DirectionalLight light, HitInfo hitInfo)
{
    vec3 L = normalize(light.Direction.xyz);
    vec3 R = normalize(reflect(L, hitInfo.Normal));
    return Phong(L,R, hitInfo.Normal, hitInfo.Point) * light.Color;
}

vec4 CalculatePerLightShading(HitInfo hitinfo)
{
    vec4 color = vec4(InstanceData[gl_InstanceCustomIndexNV].Ambient);
    if(InstanceData[gl_InstanceCustomIndexNV].HasTexture)
    {
        uint texIndex = InstanceData[gl_InstanceCustomIndexNV].Texture;
        color *= texture(TextureSamplers[texIndex], hitValue.UV);    
    }
    for(uint pointLightIndex = 0; pointLightIndex < GlobalUniform.LightCounts.y; pointLightIndex++)
    {
        color += CalculatePointLightShading(GlobalUniform.PointLights[pointLightIndex], hitinfo);
    }
    for(uint directionalLightIndex = 0; directionalLightIndex < GlobalUniform.LightCounts.x; directionalLightIndex++)
    {
        color += CalculateDirectionalLightShading(GlobalUniform.DirectionalLights[directionalLightIndex], hitinfo);
    }
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
    
    hitValue.Color = CalculatePerLightShading(hitinfo);
}