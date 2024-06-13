//
// Created by Kasper de bruin on 11/06/2024.
//

#include "WindowWithoutBorders.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <VulkanWindow.h>

namespace KBUI {
    static void renderWindowOuterBorders(ImGuiWindow *window) {
        struct ImGuiResizeBorderDef {
            ImVec2 InnerDir;
            ImVec2 SegmentN1, SegmentN2;
            float OuterAngle;
        };

        static const ImGuiResizeBorderDef resize_border_def[4] =
                {
                        ImVec2(+1, 0), ImVec2(0, 1), ImVec2(0, 0), IM_PI * 1.00f, // Left
                        ImVec2(-1, 0), ImVec2(1, 0), ImVec2(1, 1), IM_PI * 0.00f, // Right
                        ImVec2(0, +1), ImVec2(0, 0), ImVec2(1, 0), IM_PI * 1.50f, // Up
                        ImVec2(0, -1), ImVec2(1, 1), ImVec2(0, 1), IM_PI * 0.50f  // Down
                };

        auto GetResizeBorderRect = [](ImGuiWindow *window, int border_n, float perp_padding, float thickness) {
            ImRect rect = window->Rect();
            if (thickness == 0.0f) {
                rect.Max.x -= 1;
                rect.Max.y -= 1;
            }
            if (border_n == ImGuiDir_Left) {
                return ImRect(rect.Min.x - thickness,
                              rect.Min.y + perp_padding,
                              rect.Min.x + thickness,
                              rect.Max.y - perp_padding);
            }
            if (border_n == ImGuiDir_Right) {
                return ImRect(rect.Max.x - thickness,
                              rect.Min.y + perp_padding,
                              rect.Max.x + thickness,
                              rect.Max.y - perp_padding);
            }
            if (border_n == ImGuiDir_Up) {
                return ImRect(rect.Min.x + perp_padding,
                              rect.Min.y - thickness,
                              rect.Max.x - perp_padding,
                              rect.Min.y + thickness);
            }
            if (border_n == ImGuiDir_Down) {
                return ImRect(rect.Min.x + perp_padding,
                              rect.Max.y - thickness,
                              rect.Max.x - perp_padding,
                              rect.Max.y + thickness);
            }
            IM_ASSERT(0);
        };


        ImGuiContext &g = *GImGui;
        float rounding = window->WindowRounding;
        float border_size = 1.0f; // window->WindowBorderSize;
        if (border_size > 0.0f && !(window->Flags & ImGuiWindowFlags_NoBackground))
            window->DrawList->AddRect(window->Pos, {window->Pos.x + window->Size.x, window->Pos.y + window->Size.y},
                                      ImGui::GetColorU32(ImGuiCol_Border), rounding, 0, border_size);

        int border_held = window->ResizeBorderHeld;
        if (border_held != -1) {
            const ImGuiResizeBorderDef &def = resize_border_def[border_held];
            ImRect border_r = GetResizeBorderRect(window, border_held, rounding, 0.0f);
            ImVec2 p1 = ImLerp(border_r.Min, border_r.Max, def.SegmentN1);
            const float offsetX = def.InnerDir.x * rounding;
            const float offsetY = def.InnerDir.y * rounding;
            p1.x += 0.5f + offsetX;
            p1.y += 0.5f + offsetY;

            ImVec2 p2 = ImLerp(border_r.Min, border_r.Max, def.SegmentN2);
            p2.x += 0.5f + offsetX;
            p2.y += 0.5f + offsetY;

            window->DrawList->PathArcTo(p1, rounding, def.OuterAngle - IM_PI * 0.25f, def.OuterAngle);
            window->DrawList->PathArcTo(p2, rounding, def.OuterAngle, def.OuterAngle + IM_PI * 0.25f);
            window->DrawList->PathStroke(ImGui::GetColorU32(ImGuiCol_SeparatorActive), 0,
                                         ImMax(2.0f, border_size)); // Thicker than usual
        }
        if (g.Style.FrameBorderSize > 0 && !(window->Flags & ImGuiWindowFlags_NoTitleBar) && !window->DockIsActive) {
            float y = window->Pos.y + window->TitleBarHeight - 1;
            window->DrawList->AddLine(ImVec2(window->Pos.x + border_size, y),
                                      ImVec2(window->Pos.x + window->Size.x - border_size, y),
                                      ImGui::GetColorU32(ImGuiCol_Border), g.Style.FrameBorderSize);
        }
    }


    void WindowWithoutBorders::Begin(const std::string& id) {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // Disable window rounding
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // Disable window border

        const bool isMaximized = VulkanWindow::GetCurrentRenderingWindow().IsMaximized();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(-1.0f, -1.0f) : ImVec2(-1.0f, -1.0f)); // Set window padding
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // Push window border size again for consistency
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}); // Set menu bar background color

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin(id.c_str(), nullptr, window_flags);
        ImGui::PopStyleColor(); // Restore menu bar background color
        ImGui::PopStyleVar(2); // Restore window border size and padding
        ImGui::PopStyleVar(2); // Restore window rounding and border size

        {
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
            // Draw window border if the window is not maximized
            if (!isMaximized) renderWindowOuterBorders(ImGui::GetCurrentWindow());

            ImGui::PopStyleColor();
        }
    }

    void WindowWithoutBorders::End() {
        ImGui::End();
    }
} // KBUI