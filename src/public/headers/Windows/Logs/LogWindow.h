//
// Created by Kasper de bruin on 24/06/2024.
//

#ifndef KBUI_LOGWINDOW_H
#define KBUI_LOGWINDOW_H

#include <imgui.h>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

namespace KBUI::Windows {
    class LogWindow {
    public:
        LogWindow(std::string name, bool childWindow, std::filesystem::path logFile);
        ~LogWindow();

        void Begin();
        void End();
        void AddLog(const std::string &log);
        void TryReadFile();
        void StartLogFileWatcher(int intervalSeconds);
        void StopLogFileWatcher();

        void LogFileWatcher(int intervalSeconds);  // Method to continuously watch the log file

    private:
        const std::string c_Name;
        const bool c_IsChildWindow;
        const std::filesystem::path c_LogFile;
//        const float c_MenuBarHeight = 20.0f;

        long m_LastReadLine = 0;

        bool m_BeginCalled = false;
        bool m_AutoScroll = true;
        std::vector<std::string> m_LogLines;
        std::mutex m_Mutex;

        std::unique_ptr<std::thread> m_LogWatcherThread;
        std::atomic<bool> m_Running{false};
    };
}


#endif //KBUI_LOGWINDOW_H
