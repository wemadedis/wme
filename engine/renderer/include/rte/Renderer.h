#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <stdexcept>
#include <vector>


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


//make a renderer class as well. rename struct and keep it for some additional structs
namespace RTE::Renderer
{


    
//Mesh contains the vertex data info along with the transformations needed to render it properly (HOW DO WE HANDLE MULTIPLE MESHES USING SAME DATA?)
struct Mesh;
struct RenderPassInfo;
enum RenderMode { RASTERIZE, RAYTRACE };
struct RTEWindow;
//Settings related to things such as resolution, background colour, etc. (dunno what more)
struct RendererSettings;
struct Light;
struct Texture;
struct Shader;

void CreateInstance(std::string appName, VkInstance *instance, std::vector<const char *> extensions, bool enableValidationLayers);
void SetupDebugCallback(VkInstance instance);
void CreateSurface(VkInstance instance);



/*
Used to bind the window surface to the vulkan instance. Remake into a contructor since it will be a class.
*/
void Initialize(RendererSettings settings, RTEWindow window);

/*
Sets the render mode to make the renderer use either rasterization or raytracing.
Could be defined in RendererSettings but if we want to change rendering mode at run time, this is necessary.
*/
void SetRenderMode(RenderMode mode);

/*
Upload meshes to the GPU. The meshes can still be used to call the update/other kind of functions.
Can be made into single mesh upload.
*/
void UploadMeshes(std::vector<Mesh> meshes);

/*
Functions to remove individual meshes or just clearing all mesh data.
*/
void RemoveMesh(Mesh* mesh);
void ClearAllMeshData();
/*
Draw the scene based on some info given to the renderer. Does not need to be called renderpass. Though prolly best
In this info struct we define which meshes to render (when for instance frustum culling). LOOK AT SRE 😥
*/
void Draw(RenderPassInfo rpInfo);

/*
In case of raytracing, no need to provide any info to the draw call?? Or just make the previous draw take a pointer, and then pass a nullptr?
*/
void Draw();

/*
Inform the renderer that some property of the mesh has changed, thus forcing it to synchronize the data.
The renderer will the internally handle everything related to this.
*/
void MarkDirty(Mesh* mesh);


//LLLLLLLLLIIIIIIIIIIIIIIGGGGGGGGGGGHHHHHHHHHHHHTTTTTTTTTTSSSSSSSSSSSSSSSSS!!!!!!!!!!!!!!!!!!!!!!!!!
void AddLight(Light light);

void RemoveLight(Light light);

void AddTexture(Texture texture);

void RemoveTexture(Texture texture);

void UploadShader(Shader shader);

void RemoveShader(Shader shader);


};