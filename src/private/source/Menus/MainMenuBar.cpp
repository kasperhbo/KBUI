//
// Created by Kasper de bruin on 25/06/2024.
//

#include "Menus/MainMenuBar.h"
#include <imgui.h>
namespace KBUI {
    namespace Menus {
        void GetHeighest(float& heighest){
            float itemSize = ImGui::GetItemRectSize().y;
            if(itemSize > heighest){
                heighest = itemSize;
            }
        }

        void MainMenuBar::Begin() {
            ImGui::SetCursorPos(ImVec2(m_Padding, m_Padding));

            float heighest = 0;

            ImGui::Button("File");
            GetHeighest(heighest);

            ImGui::SameLine();
            ImGui::Button("Edit");
            GetHeighest(heighest);

            ImGui::SameLine();
            ImGui::Button("View");
            GetHeighest(heighest);

            ImGui::SameLine();
            ImGui::Button("Tools");
            GetHeighest(heighest);

            m_TotalHeight = heighest + (m_Padding * 2);
        }

        void MainMenuBar::End() {
            ImGui::SetCursorPos(ImVec2(0,m_TotalHeight));
        }
    } // Menus
} // KBUI