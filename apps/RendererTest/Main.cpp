#include "rte/Renderer.h"
#include "rte/Primitives.h"
#include "rte/WindowManager.h"

#include <vector>

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
    std::vector<Mesh*> meshes;
    meshes.push_back(cylinder);
    auto renderer = Renderer(info, meshes);
    while(true)
    {
        RTE::Platform::PollEvents();
        renderer.Draw();
    }
}