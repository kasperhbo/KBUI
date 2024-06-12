//
// Created by Kasper de bruin on 11/06/2024.
//

#ifndef KBUI_WINDOWWITHOUTBORDERS_H
#define KBUI_WINDOWWITHOUTBORDERS_H
#include <iostream>

namespace KBUI {

    class WindowWithoutBorders {
    public:
        WindowWithoutBorders() = default;
        virtual ~WindowWithoutBorders() = default;
        static void Begin(const std::string& id);
        static void End();
    };

} // KBUI

#endif //KBUI_WINDOWWITHOUTBORDERS_H
