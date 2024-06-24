//
// Created by Kasper de bruin on 13/06/2024.
//

#ifndef KBUI_FONTMANAGER_H
#define KBUI_FONTMANAGER_H

#include <string>
#include <filesystem>
#include <map>

#include <imgui.h>

namespace KBUI::Fonts {
    struct Font {
        std::string Name;
        std::filesystem::path Path;
        ImFont* ImGuiFont;
        float SizeInPixels;
    };
    class FontManager {

    private:
        FontManager() = default;

        ~FontManager() = default;

    public:
        static ImFontConfig GetDefaultConfig(){
            auto io = ImGui::GetIO();
            io.Fonts->Clear();
            io.Fonts->ClearInputData();
            io.Fonts->ClearTexData();

            ImFontConfig config;

            ImFontConfig baseConfig = ImFontConfig();
            baseConfig.OversampleH = 1;
            baseConfig.OversampleV = 1;
            baseConfig.SizePixels = s_defaultFontSize * s_defaultFontScale;

            ImFont *font = io.Fonts->AddFontDefault(&baseConfig);
            font->Scale = 1.0f / s_defaultFontScale;
            return baseConfig;
        }

        static std::shared_ptr<Font> LoadFont(const std::string &fontName, const std::filesystem::path &fontPath, const bool setAsDefaultFont);
        static void LoadDefaultFont(float fontSize = s_defaultFontSize, float fontScale = s_defaultFontScale);
        static void SetDefaultFont(const std::string& fontName);
        static std::shared_ptr<Font> GetFont(const std::string& fontName);
        static std::shared_ptr<Font> GetDefaultFont();
    private:
        inline static std::map<std::string, std::shared_ptr<Font>> s_Fonts = {};
        inline static std::shared_ptr<Font> s_DefaultFont = nullptr;
#ifdef __APPLE__
        inline static float s_fontSizeIncreaseFactor = 2.0f;
        inline static float s_defaultFontSize = 16.0f;
        inline static float s_defaultFontScale = 2.0f;
#else
        inline static float s_fontSizeIncreaseFactor = 1.0f;
        inline static float s_defaultFontSize = 16.0f;
        inline static float s_defaultFontScale = 1.0f;
#endif

    };
}

#endif //KBUI_FONTMANAGER_H
