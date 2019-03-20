#include "rte/Renderer.h"
#include "rte/Primitives.h"
#include "rte/WindowManager.h"

#include <iostream>
#include <vector>

#include "stb_image.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

using namespace RTE::Renderer;

int main()
{
    auto winMan = RTE::Platform::WindowManager::GetInstance();
    auto window = winMan->OpenWindow(800, 600, "RendererTest");
    auto cylinder = Primitives::MakeCylinder(1.0f, 2.0f, 36);
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
    ss << ENGINE_ASSET_DIR << "textures/aa_beauty_and_the_sun.png";

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
    cam.ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cam.ProjectionMatrix = glm::mat4(1.0f);//glm::perspective(glm::radians(180.0f), (float)800 / (float)600, 0.1f, 100.0f);
    cam.ProjectionMatrix[1][1] *= -1;

    auto renderer = Renderer(info);
    auto cylinderhandle = renderer.UploadMesh(cylinder);
    auto quadhandle = renderer.UploadMesh(quad);
    auto texture = renderer.UploadTexture(tex);
    renderer.BindTexture(texture, cylinderhandle);
    renderer.BindTexture(texture, quadhandle);
    
    renderer.SetCamera(cam);

    //renderer.SetMeshTransform(quadhandle, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(1.0f));

    renderer.Finalize();

    while(true)
    {
        RTE::Platform::PollEvents();
        renderer.Draw();
    }
}