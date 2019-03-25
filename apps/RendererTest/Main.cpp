#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include "rte/Renderer.h"
#include "rte/Primitives.h"
#include "rte/WindowManager.h"
#include <iostream>
#include <vector>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace RTE::Rendering;

int main()
{
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
    auto texture = renderer.UploadTexture(tex);
    renderer.BindTexture(texture, quadhandle);
    
    renderer.SetCamera(cam);

    PointLight p;
    p.Color = glm::vec4(0.5f);
    p.PositionRadius = glm::vec4(0.25f, 0.25f, -0.1f, 0.25f);
    PointLightHandle pl = renderer.AddPointLight(p);
    p.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);
    p.PositionRadius = glm::vec4(-0.25f, -0.25f, -0.1f, 0.25f);
    PointLightHandle pl2 = renderer.AddPointLight(p);

    p.Color = glm::vec4(0.0f,1.0f,0.0f,0.0f);
    p.PositionRadius = glm::vec4(0.25f, -0.25f, -0.1f, 0.25f);
    PointLightHandle pl3 = renderer.AddPointLight(p);

    p.Color = glm::vec4(0.0f,0.0f,1.0f,0.0f);
    p.PositionRadius = glm::vec4(-0.25f, 0.25f, -0.1f, 0.25f);
    PointLightHandle pl4 = renderer.AddPointLight(p);

    p.Color = glm::vec4(0.0f,1.0f,1.0f,0.0f);
    p.PositionRadius = glm::vec4(0.0f, 0.0f, -0.1f, 0.25f);
    PointLightHandle pl5 = renderer.AddPointLight(p);

    //Heck yes!

    DirectionalLight dirLight;
    dirLight.Color = glm::vec4(0.5f);
    dirLight.Direction = glm::normalize(glm::vec4(0.0f, -0.5f, 1.0f, 0.0f));
    auto dirLightHandle = renderer.AddDirectionalLight(dirLight);

    dirLight.Color = glm::vec4(1.0f);
    dirLight.Direction = glm::normalize(glm::vec4(0.0f, 0.5f, -1.0f, 0.0f));
    auto dirLightHandle2 = renderer.AddDirectionalLight(dirLight);

    renderer.SetAmbientLight(glm::vec4(0.1f));
    renderer.Finalize();

    while(!RTE::Platform::ShouldClose(window))
    {
        RTE::Platform::PollEvents();
        renderer.SetDirectionalLightProperties(dirLightHandle, [](DirectionalLight &light){
            light.Direction = glm::rotateX(light.Direction, glm::radians(0.05f));
        });
        renderer.SetDirectionalLightProperties(dirLightHandle2, [](DirectionalLight &light){
            light.Direction = glm::rotateX(light.Direction, glm::radians(0.05f));
        });
        renderer.SetPointLightProperties(pl, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateZ(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl2, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateZ(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl3, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateZ(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl4, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateZ(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.Draw();
    }
}