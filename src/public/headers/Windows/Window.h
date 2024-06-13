//
// Created by Kasper de bruin on 13/06/2024.
//

#ifndef KBUI_WINDOW_H
#define KBUI_WINDOW_H

#include <string>


namespace KBUI::Windows {

    class Window {
    public:
        Window() = default;

        virtual ~Window() = default;

        static void Begin(const std::string &id);

        static void End();

        static Window* Create(const std::string &id);
    };

} // Windows
// KBUI

#endif //KBUI_WINDOW_H
