//
// Created by Kasper de bruin on 13/06/2024.
//

#include "Fonts/FontManager.h"
#include <VulkanWindow.h>
#include <imgui.h>
Font KBUI::Fonts::FontManager::LoadFont(const std::string &fontName, const std::string &fontPath) {
    const float fontRenderingScale = 1.0f / s_fontSizeIncreaseFactor;
    auto io = ImGui::GetIO();
    io.FontGlobalScale = fontRenderingScale;
    io.DisplayFramebufferScale = VulkanWindow::GetWindowScaleFactor();

    io.Fonts->Clear();
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();

    ImFontConfig config;
    return Font();
}
