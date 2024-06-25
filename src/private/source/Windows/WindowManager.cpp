//
// Created by Kasper de bruin on 25/06/2024.
//

#include "Windows/WindowManager.h"

namespace KBUI::Windows {

    void WindowManager::Render() {
        for(auto window : s_Windows){
            window->Begin();
        }
    }

    void WindowManager::AddWindow(AWindow* window) {
        s_Windows.push_back(window);
    }
} // Windows
// KBUI