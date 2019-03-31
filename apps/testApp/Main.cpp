#include "rte/Renderer.h"
#include "rte/Primitives.h"
#include "rte/WindowManager.h"

#include <iostream>
#include <vector>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "rte/EntryPoint.h"
#include "RTE.h"
#include "rte/ModelImporter.h"

using namespace RTE::Rendering;

void Initialize()
{
    CreateScene();
    std::ostringstream stringStream;
    stringStream << ENGINE_ASSET_DIR << "models/nested.ply";
    std::string cubePath = stringStream.str();
    // RTE::Rendering::Mesh cube_raw = RTE::Importing::ModelImporter::ImportMesh(cubePath.c_str());
    // RTE::Rendering::Mesh *quad = &cube_raw;

    auto winMan = RTE::Platform::WindowManager::GetInstance();
    auto window = winMan->OpenWindow(800, 600, "RendererTest");
    auto quad = Primitives::MakeQuad();
    RendererInitInfo info;
    info.Width = 800;
    info.Height = 600;
    info.extensions = winMan->GetRequiredExtensions();
    info.BindingFunc = [winMan](VkSurfaceKHR &surface, VkInstance instance) {
            winMan->CreateSurface(instance, surface);
    };

    int texWidth, texHeight, texChannels;
    std::ostringstream ss;
    ss << ENGINE_ASSET_DIR << "textures/rte.png";
    stbi_uc* pixels = stbi_load(ss.str().c_str() , &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    uint32_t imageSize = texWidth * texHeight * 4;
    Texture tex;
    tex.Height = texHeight;
    tex.Width = texWidth;
    tex.Pixels = pixels;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    Camera cam;
    cam.ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cam.ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
    cam.ProjectionMatrix[1][1] *= -1;
    

    auto renderer = Renderer(info);
    auto quadhandle = renderer.UploadMesh(quad);
    auto quadInstance = renderer.CreateMeshInstance(quadhandle);
    auto texture = renderer.UploadTexture(tex);
    renderer.BindTexture(texture, quadhandle);
    renderer.SetMeshTransform(quadInstance, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f), glm::vec3(10.0f));
    renderer.SetCamera(cam);

    
    PointLight p;
    p.Color = glm::vec4(0.5f);
    p.PositionRadius = glm::vec4(0.25f, 0.25f, -0.1f, 0.25f);
    PointLightHandle pl = renderer.AddPointLight(p);
    renderer.SetAmbientLight(glm::vec4(0.5f));
    renderer.Finalize();

    while(!RTE::Platform::ShouldClose(window))
    {
        RTE::Platform::PollEvents();
        renderer.Draw();
    }

}