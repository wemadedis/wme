#version 450
#extension GL_ARB_separate_shader_objects : enable

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

struct TransformData
{
    mat4 ModelMatrix;
    mat4 NormalMatrix;
    mat4 MVPMatrix;
};

struct SurfaceData
{
    float Ambient;
    float Diffuse;
    float Specular;
    float Shininess;
    float Reflectivity;
    float Transparency;
    vec4 Color;
    uint Texture;
    bool HasTexture;
};

struct CameraData
{
    float FoV;
    float NearPlane;
    float FarPlane;
    vec4 Position;
    vec4 ClearColor;
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

struct LightData
{
    PointLight PointLights[MAX_LIGHTS];
    uint PointLightCount;
    DirectionalLight DirectionalLights[MAX_LIGHTS];
    uint DirectionalLightCount;
};

//layout(binding = 0) uniform TransformData Transform;
layout(binding = 0) uniform InstanceData {
    TransformData Transform;
    SurfaceData Surface;
} Instance;

layout(binding = 1) uniform WorldData
{
    CameraData Camera;
    LightData Lights;
    //vec4 LightCounts;
    //PointLight PointLights[MAX_LIGHTS];
    //DirectionalLight DirectionalLights[MAX_LIGHTS];
} World;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;

layout(location = 1) out vec3 N;
layout(location = 2) out vec3 V;
layout(location = 4) out vec2 UV;
layout(location = 5) out int HasTexture;
layout(location = 6) out vec3 PositionCameraSpace;



//https://learnopengl.com/Lighting/Multiple-lights
void ComputePhongProperties(){
    PositionCameraSpace = vec3(World.Camera.ViewMatrix * Instance.Transform.ModelMatrix * vec4(inPosition, 1.0));
    N = normalize(vec3(Instance.Transform.NormalMatrix * vec4(normal,0.0f)));
    V = normalize(PositionCameraSpace);
    //Flip the normal if it points away from the eye
    if(dot(N,V) > 0) N = -N;
}


void main() {
    HasTexture = Instance.Surface.HasTexture ? 1 : 0;
    gl_Position = World.Camera.ProjectionMatrix * World.Camera.ViewMatrix * Instance.Transform.ModelMatrix * vec4(inPosition, 1.0);
    ComputePhongProperties();
    fragColor = vec4(Instance.Surface.Ambient);
    UV = texCoord;
}