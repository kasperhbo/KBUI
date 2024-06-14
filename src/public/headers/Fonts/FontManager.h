//
// Created by Kasper de bruin on 13/06/2024.
//

#ifndef KBUI_FONTMANAGER_H
#define KBUI_FONTMANAGER_H
#include <string>
#include <imgui.h>

namespace KBUI::Fonts {
    class FontManager {
        FontManager() = default;

        ~FontManager() = default;
    public:
        static ImFont LoadFont(const std::string &fontName, const std::string &fontPath);
        static void LoadDefaultFont(float fontSize = s_defaultFontSize, float fontScale = s_defaultFontScale);
    private:
        inline static float s_fontSize = 16.0f;
        inline static float s_dpiWindowSizeFactor = 1.0f;
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
