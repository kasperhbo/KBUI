//
// Created by Kasper de bruin on 10/06/2024.
//

#include <VulkanWindow.h>
#include <Windows/Terminal/TerminalWindow.h>
#include <imgui.h>
#include <Fonts/FontManager.h>
#include <Windows/Logs/LogWindow.h>
#include <Menus/MainMenuBar.h>

#include <Windows/WindowManager.h>

using namespace KBUI;

int main() {
    KBUI::Properties::WindowProperties properties = {1920, 1080, "Hello, Vulkan!", ImVec4(0.45f, 0.55f, 0.60f, 1.00f)};
    KBUI::VulkanWindow vulkanWindow(properties);
    KBUI::Fonts::FontManager::LoadDefaultFont();

    auto *terminalWindow = new KBUI::Windows::TerminalWindow("terminal1", "/Users/brk");
    KBUI::Windows::WindowManager::AddWindow(terminalWindow);

    while (!vulkanWindow.ShouldClose()) {
        vulkanWindow.StartFrame();
        KBUI::Windows::WindowManager::Render();
        vulkanWindow.EndFrame();
    }
    return 0;
}