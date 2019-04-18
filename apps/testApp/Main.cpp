#include "rte/Renderer.h"
#include "rte/Primitives.h"
#include "rte/WindowManager.h"

#include <iostream>
#include <vector>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GLFW/glfw3.h>

#include "rte/EntryPoint.h"
#include "RTE.h"
#include "rte/ModelImporter.h"

using namespace RTE::Rendering;


void Initialize()
{
    CreateScene();
    std::ostringstream stringStream;
    stringStream << ENGINE_ASSET_DIR << "models/monkey.ply";
    std::string cubePath = stringStream.str();
    RTE::Rendering::Mesh cube_raw = RTE::Importing::ModelImporter::ImportMesh(cubePath.c_str());
    RTE::Rendering::Mesh *quad = &cube_raw;

    auto winMan = RTE::Platform::WindowManager::GetInstance();
    auto window = winMan->OpenWindow(800, 600, "RendererTest");


    //auto quad = Primitives::MakeQuad();
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

    glm::vec3 monkeyPosition = glm::vec3(0.0f, -10.0f, -40.0f);
    cam.ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), monkeyPosition, glm::vec3(0.0f, 1.0f, 0.0f));
    cam.ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
    cam.ProjectionMatrix[1][1] *= -1;
    
    auto renderer = Renderer(info);
    auto quadhandle = renderer.UploadMesh(quad);
    auto quadInstance = renderer.CreateMeshInstance(quadhandle);
    auto texture = renderer.UploadTexture(tex);
    renderer.SetCamera(cam);

    renderer.SetMeshTransform(quadInstance, monkeyPosition, glm::vec3(glm::radians(-90.0f), glm::radians(0.0f), 0.0f), glm::vec3(4));
    
    DirectionalLight dl;
    dl.Color = glm::vec4(0.5f, 0.5f, 0.5f, 1);
    dl.Direction = glm::vec4(-2, -2,  1, 0);
    DirectionalLightHandle dlh = renderer.AddDirectionalLight(dl);

    renderer.SetAmbientLight(glm::vec4(0.5f));
    renderer.Finalize();

    float y = 0;
    float x = 0;

    float speed;
    
    while(!RTE::Platform::ShouldClose(window))
    {
        //renderer.SetMeshTransform(quadInstance, glm::vec3(0.0f, 0.0f, -400.0f), glm::vec3(glm::radians(x), glm::radians(y), 0.0f), glm::vec3(1.0f));
        /*
        renderer.SetDirectionalLightProperties(dlh, [&](DirectionalLight &dl){
            dl.Direction = glm::rotateX(dl.Direction, glm::radians(0.1f));
            dl.Direction = glm::rotateY(dl.Direction, glm::radians(0.05f));
        });*/

        int aKeyState = glfwGetKey(window->window, GLFW_KEY_A);
        if(aKeyState == GLFW_PRESS)
        {

        }

        RTE::Platform::PollEvents();
        renderer.Draw();
    }

}