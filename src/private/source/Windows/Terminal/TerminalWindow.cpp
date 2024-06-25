//
// Created by Kasper de bruin on 24/06/2024.
//

#include "Windows/Terminal/TerminalWindow.h"
#include <imgui.h>

#pragma warning(push, 0)

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#pragma warning(pop)

namespace KBUI::Windows {
    void TerminalWindow::Begin() {
        ImGui::Begin(AWindow::c_Id.c_str(), nullptr);
        pws = getpwuid(getuid());
        if (pws == nullptr) {
            std::cerr << "Failed to get user information" << std::endl;
        }
        // Set tm background color
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));

        //Terminal area
        ImGui::BeginChild("TerminalScrollArea", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));


        //    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Last login: %s", getTimestamp().c_str());
        for (const auto & m_log : m_logs) {
            ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", m_log.GetTime().c_str());
            ImGui::SameLine();
            ImGui::TextWrapped("%s", std::string(m_log.getCommand().GetRanBy() + " " + m_log.getLog()).c_str());
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::Separator();

#ifdef __APPLE__
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0,0});
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0,0});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});

//        ImGui::SetCursorPos({0, 0});
        ImGui::Dummy(ImVec2(0, 10));
        // Command input
        ImGui::Dummy(ImVec2(10,0));
        ImGui::SameLine();
        const std::string timeStamp = GetTimestamp();
        ImGui::TextColored(ImVec4(.5f, 1.0f, 1.0f, .5f), "%s ", timeStamp.c_str());
        ImGui::SameLine();
        const std::string ranBy = pws->pw_name;
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", std::string(ranBy + ":").c_str());
#else
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", std::string(getTimestamp() + " " + "Ran by not found").c_str());
#endif

        ImGui::SameLine();
        ImGui::TextWrapped("%s", m_input.c_str());

        if ((int) (ImGui::GetTime() / 0.4) % 2) {
            ImGui::SameLine();
            ImGui::Text("|");
        }
        ImGui::PopStyleVar(3);



        //Create invisible button to focus the window
        ImGui::SetCursorPos({0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        if (ImGui::Button("##focus", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y))) {
            ImGui::SetWindowFocus();
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::SetNextItemAllowOverlap();
        HandleInput();
        ImGui::EndChild();

        ImGui::End();
    }

    std::vector<std::string> TerminalWindow::SplitCommand(const std::string &command) {
        std::vector<std::string> commands;
        std::string currentCommand;
        for (int i = 0; i < command.size(); i++) {
            if (command[i] == '&') {
                if (command[i + 1] == '&') {
                    commands.push_back(currentCommand);
                    currentCommand = "";
                    i++;
                } else {
                    currentCommand += command[i];
                }
            } else {
                currentCommand += command[i];
            }
        }
        commands.push_back(currentCommand);
        return commands;
    }

    void TerminalWindow::ExecuteCommand(const std::string &commandIn) {
        const std::vector<std::string> splittedCommand = SplitCommand(commandIn);
        for (const auto &command: splittedCommand) {
            //create a command
#ifdef __APPLE__
            Command cmd{m_currentFolder.string(), command, this->pws};
            AddLog(command, cmd);

            KillCurrentCommand();

            //run the command threaded
            commandThread = std::thread(&TerminalWindow::ExecuteCommandThreaded, this, cmd);
            commandThread.detach();
            //wait till the command is done
#else
            errorLog("Command not supported on this platform");
#endif
        }
    } // Windows

    void TerminalWindow::ExecuteCommandThreaded(const Command &cmd) {
        FILE *fp = popen(cmd.GetCommand().c_str(), "r");
        int pipeFd;

        // Create a pipe and execute the command in a child process
        if ((pipeFd = KBUI::Windows::TerminalWindow::Popen(cmd.GetCommand().c_str(), &fp)) == -1) {
            AddLog("Failed to run command: " + cmd.GetCommand(),
                   cmd);
            return;
        }

        // Store the PID of the process
        m_currentPid = pipeFd;

        char buffer[128];
        while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            // Remove newline character if present
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
            AddLog(buffer, cmd);
        }

        // Reset the PID to -1 after the command has finished
        m_currentPid = -1;
        pclose(fp);
    }

    int TerminalWindow::Popen(const char *command, FILE **fp) {
#ifdef __APPLE__
        int pipeFd[2];
        pid_t pid;

        if (pipe(pipeFd) == -1) {
            return -1;
        }

        if ((pid = fork()) == -1) {
            close(pipeFd[0]);
            close(pipeFd[1]);
            return -1;
        }

        if (pid == 0) {// child process
            close(pipeFd[0]);
            if (pipeFd[1] != STDOUT_FILENO) {
                dup2(pipeFd[1], STDOUT_FILENO);
                close(pipeFd[1]);
            }
            if (pipeFd[1] != STDERR_FILENO) {
                dup2(pipeFd[1], STDERR_FILENO);
                close(pipeFd[1]);
            }
            execl("/bin/sh", "sh", "-c", command, nullptr);
            _exit(127);
        }

        *fp = fdopen(pipeFd[0], "r");
        if (*fp == nullptr) {
            close(pipeFd[0]);
            return -1;
        }

        return pid;
#endif
        return -1;
    }
}// KBUI