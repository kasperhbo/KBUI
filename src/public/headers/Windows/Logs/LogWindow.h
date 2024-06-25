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

#include <Windows/AWindow.h>

namespace KBUI::Windows {
    class LogWindow : public AWindow {
    public:
        explicit LogWindow(const std::string &id, std::filesystem::path logFile);
        ~LogWindow() override;

        void Begin() override;
        void AddLog(const std::string &log);
        void TryReadFile();
        void StartLogFileWatcher(int intervalSeconds);
        void StopLogFileWatcher();

        void LogFileWatcher(int intervalSeconds);  // Method to continuously watch the log file

    private:
        const std::filesystem::path c_LogFile;
        long m_LastReadLine = 0;
        bool m_AutoScroll = true;
        std::vector<std::string> m_LogLines;
        std::mutex m_Mutex;

        std::unique_ptr<std::thread> m_LogWatcherThread;
        std::atomic<bool> m_Running{false};
    };
}


#endif //KBUI_LOGWINDOW_H
