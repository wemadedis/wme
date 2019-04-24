#include "rte/RenderingManager.h"
#include "rte/WindowManager.h"
#include "rte/ModelImporter.h"
#include "rte/ImportFunctions.h"
namespace RTE::Rendering
{
RenderingManager* RenderingManager::_instance = nullptr;
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
    //TRY CATCH THIS ?! .at can throw an exception
    MeshHandle mesh = _meshes.at(meshComponent->GetMesh());
    TextureHandle texture = _textures.at(meshComponent->GetTexture());

    //TODO: Set transform as well
    MeshInstanceHandle instance = _renderer->CreateMeshInstance(mesh);
    _renderer->BindTextureToMeshInstance(texture, instance);
    _instances.insert({meshComponent, instance});
    _renderer->SetMeshTransform(instance, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f), glm::vec3(1.0f));
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

}; // namespace RTE::Rendering