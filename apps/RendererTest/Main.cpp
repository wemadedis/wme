#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#define VK_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR

#include "rte/Emitter.hpp"
#include "rte/Listener.hpp"
#include "rte/ModelImporter.h"
#include "rte/Primitives.h"
#include "rte/Renderer.h"
#include "rte/WindowManager.h"
#include "rte/ModelImporter.h"

#include "stb_image.h"
#include <iostream>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

using namespace RTE::Rendering;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
using FpSeconds = std::chrono::duration<float, std::chrono::seconds::period>;

int main()
{
    RTE::RTEConfig config = {};
    config.WindowConfig.WindowWidth = 800;
    config.WindowConfig.WindowHeight = 600;
    config.WindowConfig.WindowName = "RendererTest";
    auto winMan = new RTE::Platform::WindowManager(config);
    auto quad = Primitives::MakeQuad();

    auto cylinder = Primitives::MakeCylinder(0.25f, 0.75f, 32);
    RendererInitInfo info;
    info.Width = 800;
    info.Height = 600;
    info.extensions = winMan->GetRequiredExtensions();
    info.MaxFPS = 144;
    info.RayTracingOn = true;

    info.BindingFunc = [winMan](VkSurfaceKHR &surface, VkInstance instance) {
        winMan->CreateSurface(instance, surface);
    };

    int texWidth, texHeight, texChannels;
    std::ostringstream ss;
    ss << ENGINE_ASSET_DIR << "textures/rte.png";
    stbi_uc *pixels = stbi_load(ss.str().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    uint32_t imageSize = texWidth * texHeight * 4;
    Texture tex;
    tex.Height = texHeight;
    tex.Width = texWidth;
    tex.Pixels = pixels;

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    Camera cam;
    glm::vec3 pos = {0.0f, 0.0f, 10.0f};
    cam.Position = pos;
    cam.ViewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cam.ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
    cam.ProjectionMatrix[1][1] *= -1;

    auto renderer = Renderer(info);

    //auto cylinderMeshHandle = renderer.UploadMesh(cylinder);
    auto quadhandle = renderer.UploadMesh(quad);
    auto quadInstance = renderer.CreateMeshInstance(quadhandle);
    auto quadInstance2 = renderer.CreateMeshInstance(quadhandle);

    MeshHandle cylinderMeshHandle = renderer.UploadMesh(cylinder);
    MeshInstanceHandle inst = renderer.CreateMeshInstance(cylinderMeshHandle);
    renderer.SetMeshTransform(
        inst,
        glm::vec3(5.f, 0.f, 0.f),
        glm::vec3(0.0f),
        glm::vec3(1.0f)
    );

    std::vector<MeshInstanceHandle> meshes = {};

    int width = 5;
    int height = 5;
    int depth = 5;

    for(int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int z = 0; z < depth; z++)
            {
                MeshInstanceHandle mesh = renderer.CreateMeshInstance(cylinderMeshHandle);
                renderer.SetMeshTransform(mesh, glm::vec3(x, y, z), glm::vec3(0.0f), glm::vec3(1.0f));
                meshes.push_back(mesh);
            }
        }
    }

    std::ostringstream stringStream;
    stringStream << ENGINE_ASSET_DIR << "models/monkey.ply";
    std::string monkeyPath = stringStream.str();

    auto monkey = RTE::Importing::ModelImporter::ImportMesh(monkeyPath.c_str());

    auto monkeyHandle = renderer.UploadMesh(&monkey);
    auto monkeyInstance = renderer.CreateMeshInstance(monkeyHandle);
    renderer.SetMeshTransform(monkeyInstance, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f), glm::vec3(1.0f));

    renderer.SetMeshTransform(quadInstance, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(0.0f), 0.0f, 0.0f), glm::vec3(10.0f));
    renderer.SetMeshTransform(quadInstance, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(180.0f), 0.0f, 0.0f), glm::vec3(10.0f));
    renderer.SetCamera(cam);

    PointLight p;
    p.Color = glm::vec4(0.5f);
    p.PositionRadius = glm::vec4(2.5f, 1.5f, 2.5f, 4.25f);
    PointLightHandle pl = renderer.AddPointLight(p);
    p.Color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    p.PositionRadius = glm::vec4(-2.5f, 1.5f, -2.5f, 4.25f);
    PointLightHandle pl2 = renderer.AddPointLight(p);

    p.Color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    p.PositionRadius = glm::vec4(2.5f, 1.5f, -2.5f, 4.25f);
    PointLightHandle pl3 = renderer.AddPointLight(p);

    p.Color = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    p.PositionRadius = glm::vec4(-2.5f, 1.5f, 2.5f, 4.25f);
    PointLightHandle pl4 = renderer.AddPointLight(p);

    p.Color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    p.PositionRadius = glm::vec4(0.0f, 5.5f, 0.0f, 10.25f);
    PointLightHandle pl5 = renderer.AddPointLight(p);

    renderer.SetAmbientLight(glm::vec4(0.1f));
    renderer.Finalize();
    TimePoint now = Clock::now();
    float deltaTime = 0.0f;
    bool rt = true;
    int lastPressed = 214;

    glm::vec3 monkeyRotation = glm::vec3(0.0f);

    while (!winMan->ShouldClose())
    {
        RTE::Platform::PollEvents();
        int aresult = RTE::Platform::WindowManager::GetKey(GLFW_KEY_A);
        if (aresult == GLFW_PRESS)
        {
            pos = glm::rotateY(pos, glm::radians(-50.0f)*deltaTime);
            cam.Position = pos;
            cam.ViewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        } 
        else if(RTE::Platform::WindowManager::GetKey(GLFW_KEY_D) == GLFW_PRESS)
        {
            pos = glm::rotateY(pos, glm::radians(50.0f)*deltaTime);
            cam.Position = pos;
            cam.ViewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));;
        }
        int currentPressed = RTE::Platform::WindowManager::GetKey(GLFW_KEY_R);
        if (currentPressed == GLFW_PRESS)
        {
            int wut = 0;
            if (lastPressed != GLFW_PRESS)
            {
                rt = !rt;
                if (rt)
                    renderer.SetRenderMode(RenderMode::RAYTRACE);
                else
                    renderer.SetRenderMode(RenderMode::RASTERIZE);
                lastPressed = GLFW_PRESS;
            }
        }
        else
        {
            lastPressed = 0;
        }
        //monkeyRotation.y += deltaTime*10.0f;
        //renderer.SetMeshTransform(monkeyInstance, glm::vec3(0.0f), monkeyRotation, glm::vec3(1.0f));
        
        renderer.SetPointLightProperties(pl, [&](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(30.0f) * deltaTime);
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;

            // ae.SetPosition(pos);
            // if (ae.GetState() != RTE::Audio::Playing) ae.Play();
        });
        renderer.SetPointLightProperties(pl2, [deltaTime](PointLight &light) {
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(30.0f) * deltaTime);
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl3, [deltaTime](PointLight &light) {
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(30.0f) * deltaTime);
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl4, [deltaTime](PointLight &light) {
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(30.0f) * deltaTime);
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetCamera(cam);
        renderer.Draw();
        deltaTime = std::chrono::duration_cast<FpSeconds>(Clock::now() - now).count();
        now = Clock::now();
    }
}