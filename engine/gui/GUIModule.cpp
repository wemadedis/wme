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


}