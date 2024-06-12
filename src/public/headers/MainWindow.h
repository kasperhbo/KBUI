//
// Created by Kasper de bruin on 12/06/2024.
//

#ifndef KBUI_MAINWINDOW_H
#define KBUI_MAINWINDOW_H

namespace KBUI {
    typedef int KBUI_WindowFlags;
    enum KBUI_WindowFlags_ {
        KBUI_WindowFlags_None = 0,
        KBUI_WindowFlags_HorizontalMenuBar = 1 << 0,
        KBUI_WindowFlags_VerticalMenuBar = 1 << 1,
    };

    class MainWindow {
    public:
        MainWindow() = default;
        virtual ~MainWindow() = default;

        static void Begin(KBUI_WindowFlags flags);
        static void End();

        inline static unsigned int index = 0 ;
    };

} // KBUI

#endif //KBUI_MAINWINDOW_H
