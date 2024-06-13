//
// Created by Kasper de bruin on 13/06/2024.
//

#ifndef KBUI_FONTMANAGER_H
#define KBUI_FONTMANAGER_H
#include <string>
#include "Fonts/Font.h"

namespace KBUI::Fonts {
    class FontManager {
        FontManager() = default;

        ~FontManager() = default;

        static Font LoadFont(const std::string &fontName, const std::string &fontPath);

    private:
        inline static float s_fontSize = 16.0f;
        inline static float s_dpiWindowSizeFactor = 1.0f;
#ifdef __APPLE__
        inline static float s_fontSizeIncreaseFactor = 2.0f;
#else
        inline static float s_fontSizeIncreaseFactor = 1.0f;
#endif

    };
}

#endif //KBUI_FONTMANAGER_H
