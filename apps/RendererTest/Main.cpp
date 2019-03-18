#include "rte/Renderer.h"
#include "rte/Primitives.h"
#include "rte/WindowManager.h"

#include <iostream>
#include <vector>

#include "stb_image.h"


using namespace RTE::Renderer;

int main()
{
    auto winMan = RTE::Platform::WindowManager::GetInstance();
    auto window = winMan->OpenWindow(800, 600, "RendererTest");
    auto cylinder = Primitives::MakeCylinder(1.0f, 2.0f, 36);
    RendererInitInfo info;
    info.Widht = 800;
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

    auto renderer = Renderer(info);
    auto handle = renderer.UploadMesh(cylinder);
    auto texture = renderer.UploadTexture(tex);
    renderer.BindTexture(texture, handle);
    renderer.Finalize();

    while(true)
    {
        RTE::Platform::PollEvents();
        renderer.Draw();
    }
}