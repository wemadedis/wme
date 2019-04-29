#include "rte/RenderingManager.h"
#include "rte/WindowManager.h"
#include "rte/ModelImporter.h"
#include "rte/ImportFunctions.h"
#include "rte/TransformComponent.hpp"
#include "rte/Primitives.h"
#include "imgui/imgui.h"

namespace RTE::Rendering
{
const std::string RenderingManager::QUAD = "quad";
const std::string RenderingManager::CYLINDER = "cylinder";

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
    _guiModule = new GUI::GUIModule();

    _guiModule->DrawFunction = [&](){
        ImGui::Begin("Wazzup");
        ImGui::Checkbox("RTX ON", &_rtEnabled);
        ImGui::End();
    };
    _renderer = new Renderer(info, _guiModule);
    //TODO: Handle cases where RT is requested but not available

    _textures.insert({std::string(""), Renderer::EMPTY_TEXTURE});
    UploadPrimitives();
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

    for(auto iter = _pointLights.begin(); iter != _pointLights.end(); iter++)
    {
        auto pl = iter->first;
        auto &trans = pl->GetTransformComponent()->Transform;
        _renderer->SetPointLightProperties(iter->second, [&](PointLight &light){
            light.Color = pl->Color;
            light.PositionRadius = glm::vec4(trans.Pos, pl->Radius);
        });
    }

    for(auto iter = _directionalLights.begin(); iter != _directionalLights.end(); iter++)
    {
        auto *comp = iter->first;
        _renderer->SetDirectionalLightProperties(iter->second, [&](DirectionalLight &light){
            light.Color = comp->Color;
            light.Direction = glm::vec4(comp->Direction(),0.0f);
        });
    }

    if(_mainCamera != nullptr)
    {
        UpdateMainCamera();
    }

    if(_rtEnabled) _renderer->SetRenderMode(RenderMode::RAYTRACE);
    else _renderer->SetRenderMode(RenderMode::RASTERIZE);
    
    auto size = _renderer->GetFrameSize();
    auto& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)size.x, (float)size.y);
    _renderer->Draw();
}

void RenderingManager::SetRTEnabled(bool rtEnabled)
{
    _rtEnabled = rtEnabled;
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
    _renderer->SetInstanceTransform(instance, trans.Pos, trans.Rot, trans.Scale);
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

void RenderingManager::RegisterDirectionalLight(DirectionalLightComponent *dirLight)
{
    DirectionalLight dl;
    dl.Color = dirLight->Color;
    dl.Direction = glm::vec4(dirLight->Direction(), 0.0f);
    DirectionalLightHandle light = _renderer->AddDirectionalLight(dl);
    _directionalLights.insert({dirLight, light});
}

void RenderingManager::UploadPrimitives()
{
    auto cylinder = Primitives::MakeCylinder(1, 2, 64);
    auto quad = Primitives::MakeQuad();
    MeshHandle cylinderHandle = _renderer->UploadMesh(cylinder);
    MeshHandle quadHandle = _renderer->UploadMesh(quad);
    _meshes.insert({CYLINDER, cylinderHandle});
    _meshes.insert({QUAD, quadHandle});
}

void RenderingManager::SetGUIDrawFunction(void (*function)())
{
    _guiModule->DrawFunction = function;
}

glm::ivec2 RenderingManager::GetRendererFrameSize()
{
    return _renderer->GetFrameSize();
}

}; // namespace RTE::Rendering