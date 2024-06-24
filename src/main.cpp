//
// Created by Kasper de bruin on 10/06/2024.
//

#include <VulkanWindow.h>
#include <Windows/Terminal/TerminalWindow.h>
#include <imgui.h>
#include <Fonts/FontManager.h>
#include <Windows/Logs/LogWindow.h>

using namespace KBUI;
int main() {
    KBUI::Properties::WindowProperties properties = {1920, 1080, "Hello, Vulkan!", ImVec4(0.45f, 0.55f, 0.60f, 1.00f)};
    KBUI::VulkanWindow vulkanWindow(properties);
    KBUI::Fonts::FontManager::LoadDefaultFont();
    static Windows::TerminalWindow terminalWindow("/Users/brk");
    static Windows::LogWindow logWindow("Log", false, "/opt/kcc/IsaBeheerBus/Log/IsaBeheerConsumer.log");

    while (!vulkanWindow.ShouldClose()) {
        vulkanWindow.StartFrame();
        terminalWindow.Begin("Terminal");
        Windows::TerminalWindow::End();
        logWindow.Begin();
        logWindow.End();
        vulkanWindow.EndFrame();
    }
    return 0;
}