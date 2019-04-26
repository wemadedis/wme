#include "rte/RenderingManager.h"
#include "rte/WindowManager.h"
#include "rte/ModelImporter.h"
#include "rte/ImportFunctions.h"
#include "rte/TransformComponent.hpp"

namespace RTE::Rendering
{
RenderingManager* RenderingManager::_instance = nullptr;
RenderingManager::RenderingManager(
    RTEConfig &config,
    Platform::WindowManager &windowManager)
{
    RendererInitInfo info;
    info.Width = config.WindowConfig.WindowWidth;
    info.Height = config.WindowConfig.WindowHeight;
    info.extensions = windowManager.GetRequiredExtensions();
    info.RayTracingOn = config.GraphicsConfig.UseRaytracing;
    info.BindingFunc = [&](VkSurfaceKHR &surface, VkInstance instance) {
        windowManager.CreateSurface(instance, surface);
    };

    _renderer = new Renderer(info);

    _textures.insert({std::string(""), Renderer::EMPTY_TEXTURE});

    ImportRenderingResources(config.AssetConfig.Meshes, config.AssetConfig.Textures);

    _instance = this;
}

RenderingManager::~RenderingManager()
{
}

void RenderingManager::FinalizeRenderer()
{
    _renderer->Finalize();
}

RenderingManager* RenderingManager::GetInstance()
{
    return _instance;
}

void RenderingManager::FrameResized(int32_t width, int32_t height)
{
    _renderer->FrameResized(width, height);
}

void RenderingManager::Update(float deltaTime)
{
    
    for(auto iter = _instances.begin(); iter != _instances.end(); iter++)
    {
        StdComponents::MeshComponent *comp = iter->first;
        UpdateMeshComponent(comp);
    }

    if(_mainCamera != nullptr)
    {
        UpdateMainCamera();
    }

    _renderer->Draw();
}

void RenderingManager::ImportRenderingResources(std::vector<std::string> &meshes, std::vector<std::string> &textures)
{
    for(uint32_t meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        std::string &meshPath = meshes[meshIndex];
        if(_meshes.find(meshPath) == _meshes.end())
        {
            Mesh meshData = Importing::ModelImporter::ImportMesh(meshPath.c_str());
            MeshHandle mesh = _renderer->UploadMesh(meshData);
            _meshes.insert({meshPath, mesh});
        }
    }

    for(uint32_t textureIndex = 0; textureIndex < textures.size(); textureIndex++)
    {
        std::string& texturePath = textures[textureIndex];
        if(_textures.find(texturePath) == _textures.end())
        {
            Texture texData = Importing::ImportTexture(texturePath.c_str());
            TextureHandle texture = _renderer->UploadTexture(texData);
            _textures.insert({texturePath, texture});
        }
    }
}

void RenderingManager::RegisterMeshComponent(StdComponents::MeshComponent *meshComponent)
{
    MeshHandle mesh;
    TextureHandle texture;
    try
    {
        mesh = _meshes.at(meshComponent->GetMesh());
    }
    catch(const std::out_of_range)
    {
        throw RTEException("Mesh resource not found!");
    }
    try
    {
        texture = _textures.at(meshComponent->GetTexture());
    }
    catch(const std::out_of_range)
    {
        throw RTEException("Texture resource not found!");
    }

    MeshInstanceHandle instance = _renderer->CreateMeshInstance(mesh);
    _instances.insert({meshComponent, instance});
    UpdateMeshComponent(meshComponent);   
}

void RenderingManager::UpdateMeshComponent(StdComponents::MeshComponent *meshComponent)
{
    MeshHandle mesh = _meshes.at(meshComponent->GetMesh());
    TextureHandle texture = _textures.at(meshComponent->GetTexture());
    MeshInstanceHandle instance = _instances.at(meshComponent);
    Transform &trans = meshComponent->GetTransformComponent()->Transform;
    _renderer->BindMeshToInstance(mesh, instance);
    _renderer->BindTextureToMeshInstance(texture, instance);
    _renderer->SetMeshTransform(instance, trans.Pos, trans.Rot, trans.Scale);
    _renderer->SetInstanceMaterial(instance, meshComponent->Material);
}

void RenderingManager::RegisterCameraComponent(StdComponents::CameraComponent *cc)
{
    if(_mainCamera == nullptr)
    {
        SetMainCamera(cc);
        UpdateMainCamera();
    }
    _cameras.push_back(cc);
}

void RenderingManager::SetMainCamera(StdComponents::CameraComponent *cc)
{
    if(cc == nullptr)
    {
        throw new RTEException("Main camera cannot be null!");
    }
    _mainCamera = cc;
}

void RenderingManager::UpdateMainCamera()
{
    Camera camera;
    Transform &transform = _mainCamera->GetTransformComponent()->Transform;
    camera.Position = transform.Pos;
    camera.ViewMatrix = _mainCamera->ViewMatrix();
    camera.ProjectionMatrix = _mainCamera->ProjectionMatrix();
    _renderer->SetCamera(camera);
    _renderer->SetClearColor(_mainCamera->BackgroundColor);
}

void RenderingManager::RegisterPointLight(StdComponents::PointLightComponent *pointLight)
{
    PointLight pl;
    pl.Color = pointLight->Color;
    pl.PositionRadius = glm::vec4(pointLight->GetTransformComponent()->Transform.Pos, pointLight->Radius);
    PointLightHandle light = _renderer->AddPointLight(pl);
    _pointLights.insert({pointLight, light});
}

glm::ivec2 RenderingManager::GetRendererFrameSize()
{
    return _renderer->GetFrameSize();
}

}; // namespace RTE::Rendering