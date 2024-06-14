//
// Created by Kasper de bruin on 13/06/2024.
//

#include "Fonts/FontManager.h"
#include <VulkanWindow.h>
#include <imgui.h>

ImFont KBUI::Fonts::FontManager::LoadFont(const std::string &fontName, const std::string &fontPath) {
    return ImFont();
}

void KBUI::Fonts::FontManager::LoadDefaultFont(float fontSize, float fontScale) {
    auto io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();

    ImFontConfig config;

    ImFontConfig baseConfig = ImFontConfig();
    baseConfig.OversampleH = 1;
    baseConfig.OversampleV = 1;
    baseConfig.SizePixels = fontSize * fontScale;

    ImFont *font = io.Fonts->AddFontDefault(&baseConfig);
    font->Scale = 1.0f / fontScale;

    io.DisplayFramebufferScale = VulkanWindow::GetWindowScaleFactor();
}
