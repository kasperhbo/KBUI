//
// Created by Kasper de bruin on 25/06/2024.
//

#ifndef KBUI_SERVICE_HELPER_THEMEMANAGER_H
#define KBUI_SERVICE_HELPER_THEMEMANAGER_H

#include <string>
namespace KBUI::Themes {
    enum Theme {
        THEME_ImGuiColorsClassic = 0,
        THEME_ImGuiColorsLight,
        THEME_ImGuiColorsDark,

        THEME_MaterialFlat,
        THEME_PhotoshopStyle,
        THEME_GrayVariations,
        THEME_GrayVariations_Darker,
        THEME_MicrosoftStyle,
        THEME_Cherry,
        THEME_Darcula,
        THEME_DarculaDarker,
        THEME_LightRounded,
        THEME_SoDark_AccentBlue,
        THEME_SoDark_AccentYellow,
        THEME_SoDark_AccentRed,
        THEME_BlackIsBlack,
        THEME_WhiteIsWhite,

        THEME_Count // Keep this one at the end
    };

    class ThemeManager {
    private:
        ThemeManager() = default;

        ~ThemeManager() = default;

    public:
        static void Init();

        static void SetTheme(const Theme &theme);

    private:
        static void PatchTheme();

    public:
        static Theme GetCurrentTheme() {
            return s_CurrentTheme;
        }

        static const std::string GetThemeName(const Theme &theme){
            if(theme < THEME_Count){
                return s_ThemeNames[theme];
            }
            return "";
        }

    private:
        inline static Theme s_CurrentTheme = THEME_PhotoshopStyle;
        const inline static char *s_ThemeNames[] = {
                "ImGui Classic",
                "ImGui Light",
                "ImGui Dark",
                "Material Flat",
                "Photoshop Style",
                "Gray Variations",
                "Gray Variations Darker",
                "Microsoft Style",
                "Cherry",
                "Darcula",
                "Darcula Darker",
                "Light Rounded",
                "SoDark Accent Blue",
                "SoDark Accent Yellow",
                "SoDark Accent Red",
                "Black is Black",
                "White is White"
        };
    };

} // Themes
// KBUI

#endif //KBUI_SERVICE_HELPER_THEMEMANAGER_H
