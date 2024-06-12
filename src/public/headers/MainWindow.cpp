//
// Created by Kasper de bruin on 12/06/2024.
//

#include "MainWindow.h"
#include <WindowWithoutBorders.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace KBUI {
    static std::string GetUID(){
        const std::string index = std::to_string(MainWindow::index);
        return std::string("MainWindow-" + index);
    }
    void MainWindow::Begin(KBUI_WindowFlags flags) {
        WindowWithoutBorders::Begin(GetUID());
    }

    void MainWindow::End() {
        WindowWithoutBorders::End();
    }
} // KBUI