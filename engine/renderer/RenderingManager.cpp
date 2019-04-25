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
    Camera cam;
    glm::vec3 pos = {0.0f, 0.0f, 10.0f};
    cam.Position = pos;
    cam.ViewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cam.ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
    cam.ProjectionMatrix[1][1] *= -1;
    _renderer->SetCamera(cam);

    PointLight p;
    p.Color = glm::vec4(0.5f);
    p.PositionRadius = glm::vec4(2.5f, 1.5f, 2.5f, 4.25f);
    PointLightHandle pl = _renderer->AddPointLight(p);
    _renderer->SetAmbientLight(glm::vec4(0.1f));
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
        MeshInstanceHandle instance = iter->second;
        Transform *t = &comp->GetTransformComponent()->Transform;
        _renderer->SetMeshTransform(instance, t->Pos, t->Rot, t->Scale);
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
    _renderer->BindTextureToMeshInstance(texture, instance);
    _instances.insert({meshComponent, instance});
    //TODO: Set transform as well
    auto& trans = meshComponent->GetTransformComponent()->Transform;
    _renderer->SetMeshTransform(instance, trans.Pos, trans.Rot, trans.Scale);
}

void RenderingManager::UpdateMeshComponent(StdComponents::MeshComponent *meshComponent)
{
    MeshHandle mesh = _meshes.at(meshComponent->GetMesh());
    TextureHandle texture = _textures.at(meshComponent->GetTexture());
    MeshInstanceHandle instance = _instances.at(meshComponent);
    _renderer->BindMeshToInstance(mesh, instance);
    _renderer->BindTextureToMeshInstance(texture, instance);
    //TODO: Set transform as well
}

void RenderingManager::RegisterCameraComponent(StdComponents::CameraComponent *cc)
{
    Camera camera;
    Transform &transform = cc->GetTransformComponent()->Transform;
    camera.Position = transform.Pos;
    camera.ViewMatrix = transform.RotationMatrix() * transform.TranslationMatrix();
    camera.ProjectionMatrix = glm::perspective(cc->FieldOfView, (float)_renderer->GetFrameWidth() / (float)_renderer->GetFrameHeight(), cc->NearPlane, cc->FarPlane);
    if(_mainCamera == nullptr)
    {
        _mainCamera = cc;
        _renderer->SetCamera(camera);
    }
    //HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
}

void RenderingManager::SetMainCameraComponent(StdComponents::CameraComponent *cc)
{
    if(cc == nullptr)
    {
        throw new RTEException("Main camera cannot be null!");
    }
    _mainCamera = cc;
}

void RenderingManager::UpdateMainCamera()
{

}

}; // namespace RTE::Rendering