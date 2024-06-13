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
        std::string UID = GetUID();
        WindowWithoutBorders::Begin(UID);

        // Dockspace
        ImGuiIO &io = ImGui::GetIO();
        ImGuiStyle &style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        ImGui::DockSpace(ImGui::GetID(std::string(UID + "MyDockspace").c_str()));
        style.WindowMinSize.x = minWinSizeX;
    }

    void MainWindow::End() {
        WindowWithoutBorders::End();
    }
} // KBUI