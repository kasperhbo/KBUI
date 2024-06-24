//
// Created by Kasper de bruin on 24/06/2024.
//

#ifndef KBUI_TERMINALWINDOW_H
#define KBUI_TERMINALWINDOW_H

#include <csignal>
#include <mutex>

#include <thread>
#include <utility>
#include <zconf.h>

#ifdef __APPLE__

#include <pwd.h>

#endif

#include <imgui.h>
#include <filesystem>

namespace KBUI::Windows {
    struct Command {
    public:
        explicit Command(std::string location, std::string command, const passwd *pws) :
                location(std::move(location)),
                command(std::move(command)),
                pws(pws) {
        }

        [[nodiscard]] std::string GetCommand() const { return command; }

        [[nodiscard]] std::string GetLocation() const { return location; }

        [[nodiscard]] std::string GetRanBy() const {
#ifdef __APPLE__
            if (pws != nullptr) {
                return pws->pw_name;
            }
#endif
            return "Ran by not found";
        }

    private:
        const std::string location;
        const std::string command;
#ifdef __APPLE__
        //ran by
        const passwd *pws = nullptr;
#endif
    };

    struct TerminalLog {
    public:
        TerminalLog(std::string time, std::string log, const Command &command) : time(std::move(time)),
                                                                                 log(std::move(log)),
                                                                                 command(command) {
        }

        ~TerminalLog() = default;

        [[nodiscard]] std::string GetTime() const { return time; }

        [[nodiscard]] std::string getLog() const { return log; }

        [[nodiscard]] Command getCommand() const { return command; }

    private:
        std::string time;
        std::string log;
        Command command;
    };

    class TerminalWindow {
    public:
        TerminalWindow(const std::filesystem::path &currentFolder) : m_currentFolder(currentFolder) {
#ifdef __APPLE__
            pws = getpwuid(getuid());
#endif
        }

        ~TerminalWindow() = default;

        void Begin(const std::string &id);

        static void End();

        static std::vector<std::string> SplitCommand(const std::string &command);

        void ExecuteCommand(const std::string &command);

        void ExecuteCommandThreaded(const Command &cmd);

        /**
         * @brief Custom popen function to execute commands
         * @param command The command to execute
         * @param fp The file pointer to write the output to
         * @return The exit code of the command
         */
        static int Popen(const char *command, FILE **fp);

    private:
        void AddLog(const std::string &log, const Command &command) {
            std::lock_guard<std::mutex> lock(m_logMutex);
            m_logs.emplace_back(GetTimestamp(), log, command);
        }

        void eErrorLog(const std::string &log) {
            std::lock_guard<std::mutex> lock(m_logMutex);
            m_logs.emplace_back(GetTimestamp(), log, Command("", "", this->pws));
//            CORE_ERROR(log);
        }

        void KillCurrentCommand() {
            pid_t pid = m_currentPid.load();
            if (pid != -1) {
#ifdef __APPLE__
                kill(pid, SIGTERM);// sends a termination signal
                m_currentPid = -1; // reset the current PID
#endif
            }
        }

        //TODO: USE INPUT CLASS
        void HandleInput() {
            if (ImGui::IsItemFocused() || ImGui::IsWindowFocused()) {
                //record kb input and add to input string
                bool inputChange = !ImGui::GetIO().InputQueueCharacters.empty();
                if (inputChange) {
                    ScrollToBottom();
                    int inputChar = ImGui::GetIO().InputQueueCharacters.front();
                    if (inputChar != 0) {
                        m_input += (char) inputChar;
                    }
                }

                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace))) {
                    ScrollToBottom();
                    if (!m_input.empty()) {
                        m_input.pop_back();
                    }
                }

                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
                    ScrollToBottom();
                    ExecuteCommand(m_input);
                    m_input = "";
                }

                //todo: implement up for history etc.
            }
        }

        static void ScrollToBottom() {
            float maxy = ImGui::GetScrollMaxY();
            ImGui::SetScrollY(maxy);
        }

        static std::string GetTimestamp() {
            std::time_t now = std::time(nullptr);
            std::tm *ltm = std::localtime(&now);
            char timestamp[10];
            std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", ltm);
            return {timestamp};
        }

    private:
        std::vector<std::string> m_commandQueue;
        std::mutex m_logMutex;
        std::vector<TerminalLog> m_logs;
        std::string m_input;
        std::atomic<pid_t> m_currentPid = -1;// using atomic for thread-safety
        std::thread commandThread;

        std::filesystem::path m_currentFolder;

        //User data
        //get the user name
#ifdef __APPLE__
        struct passwd *pws = nullptr;
#endif
    };

} // Windows
// KBUI

#endif //KBUI_TERMINALWINDOW_H
