//
// Created by Kasper de bruin on 25/06/2024.
//

#ifndef KBUI_MENUITEM_H
#define KBUI_MENUITEM_H

#include <string>
#include <functional>

namespace KBUI {
    namespace Menus {

        struct MenuItem {
            std::string Name;
            std::string Description;
            std::function<void()> Action;
        };

    } // Menus
} // KBUI

#endif //KBUI_MENUITEM_H
