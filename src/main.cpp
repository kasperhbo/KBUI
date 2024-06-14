//
// Created by Kasper de bruin on 10/06/2024.
//

#include <VulkanWindow.h>
#include <imgui.h>
#include <Fonts/FontManager.h>

int main() {
    KBUI::Properties::WindowProperties properties = {1920, 1080, "Hello, Vulkan!", ImVec4(0.45f, 0.55f, 0.60f, 1.00f)};
    KBUI::VulkanWindow mainWindow(properties);
    KBUI::Fonts::FontManager::LoadDefaultFont();

    while (!mainWindow.ShouldClose()) {
        mainWindow.StartFrame();
        ImGui::ShowDemoWindow();
        mainWindow.EndFrame();
    }
    return 0;
}