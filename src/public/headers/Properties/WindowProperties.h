//
// Created by Kasper de bruin on 10/06/2024.
//

#ifndef KBUI_WINDOWPROPERTIES_H
#define KBUI_WINDOWPROPERTIES_H

#include <iostream>

namespace KBUI::Properties {
    class WindowProperties {
    public:
        WindowProperties(int width, int height, std::string title)
                : m_Width(width), m_Height(height), m_Title(std::move(title)) {}

        WindowProperties(int width, int height, std::string title, ImVec4 clearColor)
                : m_Width(width), m_Height(height), m_Title(std::move(title)), m_clearColor(clearColor) {}

        [[nodiscard]]int getWidth() const { return m_Width; }

        [[nodiscard]]int getHeight() const { return m_Height; }

        [[nodiscard]]const std::string &getTitle() const { return m_Title; }

        [[nodiscard]]ImVec4 getClearColor() {
            return m_clearColor;
        }

        [[nodiscard]]ImVec4 &getClearColorRef() {
            return m_clearColor;
        }

    public:

    private:
        int m_Width;
        int m_Height;
        std::string m_Title;
        ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    };
}

#endif //KBUI_WINDOWPROPERTIES_H
