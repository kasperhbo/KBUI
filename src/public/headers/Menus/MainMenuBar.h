//
// Created by Kasper de bruin on 25/06/2024.
//

#ifndef KBUI_MAINMENUBAR_H
#define KBUI_MAINMENUBAR_H
#include <iostream>
#include <Menus/MenuItem.h>

namespace KBUI {
    namespace Menus {

        class MainMenuBar {
        private:
            MainMenuBar() = default;
            ~MainMenuBar() = default;
        public:
            static void Begin();//todo: make static
            static void End();
        private:
            static std::vector<MenuItem> items;
            inline static float m_TotalHeight = 0;
            inline static float m_Padding = 10;
        };

    } // Menus
} // KBUI

#endif //KBUI_MAINMENUBAR_H
