#include "rte/GUIModule.h"
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_vulkan.h"
#include "rte/WindowManager.h"

namespace RTE::GUI
{


void GUIModule::Initialize(GUIInitInfo info, VkRenderPass rp, VkCommandBuffer cmdBuffer)
{
    ImGui::CreateContext();
    ImGui_ImplVulkan_Init((ImGui_ImplVulkan_InitInfo*)&info, rp);
    ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);
    ImGui::StyleColorsDark();
    SetupInputCallbacks();
}

void GUIModule::Draw(VkCommandBuffer cmdBuffer, uint32_t frameWidth, uint32_t frameHeight)
{
    auto& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)frameWidth, (float)frameHeight);
    ImGui::NewFrame();
    if(ImGUIDrawCommands != nullptr)
    {
        ImGUIDrawCommands();
    }
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
    ImGui::EndFrame();
}

void GUIModule::SetupInputCallbacks()
{
    auto& io = ImGui::GetIO();
    auto wm = Platform::WindowManager::GetInstance();
    wm->RegisterMouseButtonCallback([&](int button, int action){
        if(button >= 0 && button < 3)
        {
            io.MouseDown[button] = action == GLFW_PRESS;
        }
    });

    wm->RegisterMousePositionCallback([&](double x, double y){
        io.MousePos = ImVec2(x,y);
    });

    wm->RegisterMouseWheelCallback([&](double xoffset, double yoffset){
        io.MouseWheelH += (float)xoffset;
        io.MouseWheel += (float)yoffset;
    });

    wm->RegisterKeyCallback([&](int key, int action){
        io.KeysDown[key] = action == GLFW_PRESS;
    });

    wm->RegisterCharCallback([&](unsigned int c){
        io.AddInputCharacter((unsigned short)c);
    });

}


}