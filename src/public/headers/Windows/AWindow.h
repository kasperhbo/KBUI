//
// Created by Kasper de bruin on 13/06/2024.
//

#ifndef KBUI_WINDOW_H
#define KBUI_WINDOW_H

#include <string>
#include <utility>
#include <iostream>


namespace KBUI::Windows {

    class AWindow {
    public:
        explicit AWindow(std::string  id) : c_Id(std::move(id)) {
            std::cout << "Creating window with id: " << c_Id << std::endl;
        }

        virtual ~AWindow() = default;

        virtual void Begin() = 0;
    protected:
        const std::string c_Id;
    };

} // Windows
// KBUI

#endif //KBUI_WINDOW_H
