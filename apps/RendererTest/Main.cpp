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


#include "rte/HighLevelRenderer.h"

using namespace RTE::Rendering;

int main()
{
    auto winMan = RTE::Platform::WindowManager::GetInstance();
    auto window = winMan->OpenWindow(800, 600, "RendererTest");
    auto quad = Primitives::MakeQuad();
    auto cylinder = Primitives::MakeCylinder(0.25f, 0.75f, 32);
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
    cam.ViewMatrix = glm::lookAt(glm::vec3(2.5f, 2.5f, 20.0f), glm::vec3(2.5f, 2.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cam.ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
    cam.ProjectionMatrix[1][1] *= -1;


    auto renderer = Renderer(info);
    auto quadhandle = renderer.UploadMesh(quad);
    auto quadInstance = renderer.CreateMeshInstance(quadhandle);
    auto cylinderMeshHandle = renderer.UploadMesh(cylinder);


    std::vector<MeshInstanceHandle> meshes = {};

    int width = 5;
    int height = 5;
    int depth = 5;

    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            for(int z = 0; z < depth; z++)
            {
                MeshInstanceHandle mesh = renderer.CreateMeshInstance(cylinderMeshHandle);
                renderer.SetMeshTransform(mesh, glm::vec3(x,y,z), glm::vec3(0.0f), glm::vec3(1.0f));
                meshes.push_back(mesh);
            }
        }
    }

    /*
    auto cylinderhandle1 = renderer.CreateMeshInstance(cylinderMeshHandle);
    auto cylinderhandle2 = renderer.CreateMeshInstance(cylinderMeshHandle);
    auto cylinderhandle3 = renderer.CreateMeshInstance(cylinderMeshHandle);
    auto cylinderhandle4 = renderer.CreateMeshInstance(cylinderMeshHandle);

    auto texture = renderer.UploadTexture(tex);
    
    renderer.BindTexture(texture, quadInstance);
    //renderer.BindTexture(texture,cylinderhandle1);
    //renderer.BindTexture(texture,cylinderhandle2);
    //renderer.BindTexture(texture,cylinderhandle3);
    //renderer.BindTexture(texture,cylinderhandle4);

    renderer.SetMeshTransform(cylinderhandle1, glm::vec3(2.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    renderer.SetMeshTransform(cylinderhandle2, glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    renderer.SetMeshTransform(cylinderhandle3, glm::vec3(0.0f, 0.0f, -2.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    renderer.SetMeshTransform(cylinderhandle4, glm::vec3(-2.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    */
    renderer.SetMeshTransform(quadInstance, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f), glm::vec3(10.0f));
    renderer.SetCamera(cam);

    PointLight p;
    p.Color = glm::vec4(0.5f);
    p.PositionRadius = glm::vec4(2.5f, 1.5f, 2.5f, 4.25f);
    PointLightHandle pl = renderer.AddPointLight(p);
    p.Color = glm::vec4(1.0f,0.0f,0.0f,0.0f);
    p.PositionRadius = glm::vec4(-2.5f, 1.5f, -2.5f, 4.25f);
    PointLightHandle pl2 = renderer.AddPointLight(p);

    p.Color = glm::vec4(0.0f,1.0f,0.0f,0.0f);
    p.PositionRadius = glm::vec4(2.5f, 1.5f, -2.5f, 4.25f);
    PointLightHandle pl3 = renderer.AddPointLight(p);

    p.Color = glm::vec4(0.0f,0.0f,1.0f,0.0f);
    p.PositionRadius = glm::vec4(-2.5f, 1.5f, 2.5f, 4.25f);
    PointLightHandle pl4 = renderer.AddPointLight(p);

    //Heck yes!
/*
    DirectionalLight dirLight;
    dirLight.Color = glm::vec4(0.5f);
    dirLight.Direction = glm::normalize(glm::vec4(0.0f, -0.5f, 1.0f, 0.0f));
    auto dirLightHandle = renderer.AddDirectionalLight(dirLight);
/*
    dirLight.Color = glm::vec4(1.0f);
    dirLight.Direction = glm::normalize(glm::vec4(0.0f, 0.5f, -1.0f, 0.0f));
    auto dirLightHandle2 = renderer.AddDirectionalLight(dirLight);
*/
    renderer.SetAmbientLight(glm::vec4(0.1f));
    renderer.Finalize();

    while(!RTE::Platform::ShouldClose(window))
    {
        RTE::Platform::PollEvents();
        /*renderer.SetDirectionalLightProperties(dirLightHandle, [](DirectionalLight &light){
            light.Direction = glm::rotateX(light.Direction, glm::radians(0.25f));
        });
        /*
        renderer.SetDirectionalLightProperties(dirLightHandle2, [](DirectionalLight &light){
            light.Direction = glm::rotateX(light.Direction, glm::radians(0.05f));
        });*/
        renderer.SetPointLightProperties(pl, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl2, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl3, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.SetPointLightProperties(pl4, [](PointLight &light){
            glm::vec3 pos = glm::vec3(light.PositionRadius.x, light.PositionRadius.y, light.PositionRadius.z);
            pos = glm::rotateY(pos, glm::radians(0.05f));
            light.PositionRadius.x = pos.x;
            light.PositionRadius.y = pos.y;
            light.PositionRadius.z = pos.z;
        });
        renderer.Draw();
    }
}