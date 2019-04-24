#include "rte/RenderingManager.h"
#include "rte/WindowManager.h"
#include "rte/ModelImporter.h"
#include "rte/ImportFunctions.h"
namespace RTE::Rendering
{

RenderingManager::RenderingManager(
    RTEConfig &config,
    Platform::WindowManager &windowManager)
{
    RendererInitInfo info;
    info.Width = config.WindowConfig.WindowWidth;
    info.Height = config.WindowConfig.WindowWidth;
    info.extensions = windowManager.GetRequiredExtensions();
    info.RayTracingOn = config.GraphicsConfig.UseRaytracing;
    info.BindingFunc = [&](VkSurfaceKHR &surface, VkInstance instance) {
        windowManager.CreateSurface(instance, surface);
    };

    _renderer = new Renderer(info);
    _renderer->Finalize();
}

RenderingManager::~RenderingManager()
{
}

void RenderingManager::FrameResized(int32_t width, int32_t height)
{
    _renderer->FrameResized(width, height);
}

void RenderingManager::Update(float deltaTime)
{
    _renderer->Draw();
    //! Prolly do something maybe
}

void RenderingManager::RegisterMeshComponent(StandardComponents::MeshComponent *meshComponent)
{
    MeshHandle mesh;
    std::string& meshPath = meshComponent->_meshPath;
    if(_meshes.find(meshPath) != _meshes.end())
    {
        mesh = _meshes[meshPath];
    }
    else
    {
        Mesh meshData = Importing::ModelImporter::ImportMesh(meshPath.c_str());
        mesh = _renderer->UploadMesh(meshData);
        _meshes.insert({meshPath, mesh});
    }
    MeshInstanceHandle instance = _renderer->CreateMeshInstance(mesh);
    _instances.insert({meshComponent, instance});
}

void RenderingManager::UpdateMeshComponent(StandardComponents::MeshComponent *meshComponent)
{
    
    MeshHandle mesh;
    std::string& meshPath = meshComponent->_meshPath;
    TextureHandle texture;
    std::string& texPath = meshComponent->_texturePath;

    if(_meshes.find(meshPath) != _meshes.end())
    {
        mesh = _meshes[meshPath];
    }
    else
    {
        Mesh meshData = Importing::ModelImporter::ImportMesh(meshPath.c_str());
        mesh = _renderer->UploadMesh(meshData);
        _meshes.insert({meshPath, mesh});
    }
    MeshInstanceHandle instance = _renderer->CreateMeshInstance(mesh);
        
    if(_textures.find(texPath) != _textures.end())
    {
        texture = _textures[texPath];
    }
    else
    {
        Texture texData = Importing::ImportTexture(texPath.c_str());
        texture = _renderer->UploadTexture(texData);
        _textures.insert({texPath, texture});
    }
}

}; // namespace RTE::Rendering