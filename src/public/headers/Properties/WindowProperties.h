//
// Created by Kasper de bruin on 10/06/2024.
//

#ifndef KBUI_WINDOWPROPERTIES_H
#define KBUI_WINDOWPROPERTIES_H

#include <iostream>

class WindowProperties {
public:
    WindowProperties(int width, int height, std::string title)
            : m_Width(width), m_Height(height), m_Title(std::move(title)) {}

    WindowProperties(int width, int height, std::string title, ImVec4 clearColor)
            : m_Width(width), m_Height(height), m_Title(std::move(title)), m_clearColor(clearColor) {}

    int getWidth() const { return m_Width; }

    int getHeight() const { return m_Height; }

    const std::string &getTitle() const { return m_Title; }

    ImVec4 getClearColor() {
        return m_clearColor;
    }

    ImVec4& getClearColorRef() {
        return m_clearColor;
    }

public:

private:
    int m_Width;
    int m_Height;
    std::string m_Title;
    ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

#endif //KBUI_WINDOWPROPERTIES_H
