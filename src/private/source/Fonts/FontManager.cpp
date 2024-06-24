//
// Created by Kasper de bruin on 13/06/2024.
//

#include "Fonts/FontManager.h"
#include <VulkanWindow.h>
#include <imgui.h>

namespace KBUI::Fonts {
    Font& KBUI::Fonts::FontManager::LoadFont(const std::string &fontName, const std::filesystem::path &fontPath, const bool setAsDefault) {
        auto io = ImGui::GetIO();
        ImFontConfig config = GetDefaultConfig();

        ImFont *imFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), s_defaultFontSize * s_fontSizeIncreaseFactor, &config);
        imFont->Scale = 1.0f / s_defaultFontScale;

        std::shared_ptr<Font> font = std::make_shared<Font>();
        font->Name = fontName;
        font->Path = fontPath;
        font->ImGuiFont = imFont;
        font->SizeInPixels = config.SizePixels;

        s_Fonts[fontName] = font;
        if(setAsDefault){
            std::cout << "set default" << std::endl;
            SetDefaultFont(fontName);
        }
        return *font;
    }

    void KBUI::Fonts::FontManager::LoadDefaultFont(float fontSize, float fontScale) {
        auto io = ImGui::GetIO();
        ImFontConfig config = GetDefaultConfig();
        ImFont *font = io.Fonts->AddFontDefault(&config);
        font->Scale = 1.0f / fontScale;
        io.DisplayFramebufferScale = VulkanWindow::GetWindowScaleFactor();
    }

    void FontManager::SetDefaultFont(const std::string &fontName) {
        std::shared_ptr<Font> font = GetFont(fontName);
        if(font != nullptr){
            auto io = ImGui::GetIO();
            io.FontDefault = font->ImGuiFont;
        }else{
            std::cerr << fontName << " not found" << std::endl;
        }
    }

    std::shared_ptr<Font> FontManager::GetFont(const std::string &fontName) {
        auto font = s_Fonts[fontName];
        return font;
    }
}