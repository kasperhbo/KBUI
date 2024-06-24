//
// Created by Kasper de bruin on 24/06/2024.
//

#include "Windows/Logs/LogWindow.h"

namespace KBUI::Windows {
    std::string GetLineAtIndex() {
//        // Skip lines until reaching the desired index
//        for (int currentLine = 0; std::getline(file, line) && currentLine < index; ++currentLine) {
//            // Just iterating through
//        }
//
//        // Check if the desired line was read successfully
//        if (file && line.empty() && index > 0) {
//            // We reached EOF without finding enough lines
//            return "";
//        }
//
//        return line;
    }

    LogWindow::LogWindow(std::string name, bool childWindow, std::filesystem::path logFile)
            : c_Name(std::move(name)), c_IsChildWindow(childWindow), c_LogFile(std::move(logFile)) {
        StartLogFileWatcher(1);
    }

    void LogWindow::StartLogFileWatcher(int intervalSeconds) {
        if (m_Running) {
            return; // If already running, do nothing
        }
        m_Running = true;
        m_LogWatcherThread = std::make_unique<std::thread>(&LogWindow::LogFileWatcher, this, intervalSeconds);
    }

    void LogWindow::StopLogFileWatcher() {
        m_Running = false; // Signal the thread to stop
        if (m_LogWatcherThread && m_LogWatcherThread->joinable()) {
            m_LogWatcherThread->join(); // Wait for the thread to finish
        }
    }


    void LogWindow::LogFileWatcher(int intervalSeconds) {
        while (m_Running) {
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
            TryReadFile();
        }
    }

    LogWindow::~LogWindow() {
        StopLogFileWatcher();
    }

    void LogWindow::Begin() {
        ImGui::Begin(c_Name.c_str());

        //////////////////////////
        //////// Menu Bar ////////
        //////////////////////////
        {
            if (ImGui::Button("Clear")) {
                m_LogLines.clear();
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Auto-scroll", &m_AutoScroll)) {
                if (m_AutoScroll) {
                    ImGui::SetScrollHereY(1.0f);
                } // If auto-scroll is enabled, scroll to the bottom
            }

        }

        ////////////////////////////
        //////// Log Window ////////
        ////////////////////////////
        {
            ImGui::BeginChild("LogChild", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            m_BeginCalled = true;
            for (const auto &line: m_LogLines) {
                ImGui::TextUnformatted(line.c_str());
            }
            if (m_AutoScroll) {
                ImGui::SetScrollHereY(1.0f); // Auto-scroll
            }
            bool reachedBottom = ImGui::GetScrollY() == ImGui::GetScrollMaxY();

            ImGui::EndChild();
            if(ImGui::IsItemHovered()){
               if(ImGui::GetIO().MouseWheel != 0){
                   if(reachedBottom){
                          m_AutoScroll = true;
                   }else {
                       m_AutoScroll = false;
                   }
               }
            }
        }
    }

    void LogWindow::End() {
        ImGui::End();
        m_BeginCalled = false;
    }

    void LogWindow::AddLog(const std::string &log) {
        m_LogLines.push_back(log);
    }

    void LogWindow::TryReadFile() {
        std::lock_guard<std::mutex> lock(m_Mutex);  // Ensure thread safety
        std::ifstream file(c_LogFile);

        if (!file.is_open()) {
            std::cerr << "Failed to open log file: " << c_LogFile << std::endl;
            return;
        }

        const long lineCount = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
        if (m_LastReadLine >= lineCount) {
            file.close();
            return;
        }

        // Seek to the last known position
        file.seekg(m_LastReadLine);  // Note: m_LastReadPos must be a std::streampos, not a line count

        std::string line;
        int currentLine = 0;
        while (std::getline(file, line)) {
            if (currentLine < m_LastReadLine) {
                currentLine++;
                continue;
            }
            AddLog(line);
        }

        // Update the last read position to the current position in the file
        m_LastReadLine = lineCount;

        file.close();
    }
}