#include "rte/GUIModule.h"
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_vulkan.h"


namespace RTE::GUI
{

void GUIModule::Initialize(GUIInitInfo info, VkRenderPass rp, VkCommandBuffer cmdBuffer)
{
    ImGui::CreateContext();
    ImGui::GetIO();
    ImGui_ImplVulkan_Init((ImGui_ImplVulkan_InitInfo*)&info, rp);
    ImGui::StyleColorsDark();
}

void GUIModule::Draw(VkCommandBuffer cmdBuffer)
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);

}


}