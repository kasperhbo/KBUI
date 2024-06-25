//
// Created by Kasper de bruin on 25/06/2024.
//

#ifndef KBUI_SERVICE_HELPER_WINDOWMANAGER_H
#define KBUI_SERVICE_HELPER_WINDOWMANAGER_H
#include <vector>
#include <Windows/AWindow.h>

namespace KBUI::Windows {

    class WindowManager {
    private:
        WindowManager() = default;
        ~WindowManager() = default;
    public:
        static void Render();
        static void AddWindow(AWindow* window);

    private:
        inline static std::vector<AWindow*> s_Windows;
    };

} // Windows

#endif //KBUI_SERVICE_HELPER_WINDOWMANAGER_H
