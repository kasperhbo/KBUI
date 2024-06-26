#include <VulkanWindow.h>

#include <iostream>
#include <utility>
#include "MainWindow.h"
#include <Fonts/FontManager.h>
#include <Menus/MainMenuBar.h>
#include <Theme/ThemeManager.h>

namespace KBUI {
    VulkanWindow::VulkanWindow(Properties::WindowProperties windowProperties) :
            m_WindowProperties(std::move(windowProperties)),
            m_Allocator(nullptr),
            m_SwapChainRebuild(false),
            m_MinImageCount(2),
            m_window(nullptr) {

        if (!Initialize()) {
            std::cerr << "Failed to initialize" << std::endl;
            abort();
        }

        MainLoop();
    }

    VulkanWindow::~VulkanWindow() {
        VkResult err = vkDeviceWaitIdle(m_Device);
        check_vk_result(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        CleanupVulkanWindow();
        CleanupVulkan();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    bool VulkanWindow::Initialize() {
        glfwSetErrorCallback(glfw_error_callback);
        if (!InitializeGLFW())
            return false;

        m_window = glfwCreateWindow(m_WindowProperties.getWidth(), m_WindowProperties.getHeight(),
                                    m_WindowProperties.getTitle().c_str(), nullptr, nullptr);

        if (!m_window)
            return false;

        if (!InitializeVulkan())
            return false;

        SetupDearImGui();
        return true;
    }

    void VulkanWindow::StartFrame() {
        s_CurrentRenderingWindow = this;
        glfwPollEvents();

        int fb_width, fb_height;
        glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
        if (fb_width > 0 && fb_height > 0 &&
            (m_SwapChainRebuild || m_MainWindowData.Width != fb_width || m_MainWindowData.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(m_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device, &m_MainWindowData,
                                                   m_QueueFamily, m_Allocator, fb_width, fb_height,
                                                   m_MinImageCount);
            m_MainWindowData.FrameIndex = 0;
            m_SwapChainRebuild = false;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (Fonts::FontManager::GetDefaultFont()) {
            ImGui::PushFont(Fonts::FontManager::GetDefaultFont()->ImGuiFont);
        }

        MainWindow::Begin(KBUI_WindowFlags_None);

    }

    void VulkanWindow::EndFrame() {
        MainWindow::End();

        ImGuiIO &io = ImGui::GetIO();

        if (KBUI::Fonts::FontManager::GetDefaultFont()) {
            ImGui::PopFont();
        }

        ImGui::Render();
        ImDrawData *main_draw_data = ImGui::GetDrawData();
        m_MainWindowData.ClearValue.color.float32[0] =
                m_WindowProperties.getClearColor().x * m_WindowProperties.getClearColor().w;

        m_MainWindowData.ClearValue.color.float32[1] =
                m_WindowProperties.getClearColor().y * m_WindowProperties.getClearColor().w;

        m_MainWindowData.ClearValue.color.float32[2] =
                m_WindowProperties.getClearColor().z * m_WindowProperties.getClearColor().w;

        m_MainWindowData.ClearValue.color.float32[3] = m_WindowProperties.getClearColor().w;

        if (!IsMinimized()) {
            FrameRender(&m_MainWindowData, main_draw_data);
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        if (!IsMinimized()) {
            FramePresent(&m_MainWindowData);
        }
    }

    bool VulkanWindow::InitializeGLFW() {
        if (!glfwInit())
            return false;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        return true;
    }

    bool VulkanWindow::InitializeVulkan() {
        if (!glfwVulkanSupported()) {
            std::cerr << "GLFW: Vulkan Not Supported" << std::endl;
            return false;
        }

        ImVector<const char *> extensions;
        uint32_t extensions_count = 0;
        const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
        for (uint32_t i = 0; i < extensions_count; i++)
            extensions.push_back(glfw_extensions[i]);
        SetupVulkan(extensions);

        VkSurfaceKHR surface;
        VkResult err = glfwCreateWindowSurface(m_Instance, m_window, m_Allocator, &surface);
        check_vk_result(err);

        int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
        ImGui_ImplVulkanH_Window *wd = &m_MainWindowData;
        SetupVulkanWindow(wd, surface, w, h);

        return true;
    }

    void VulkanWindow::SetupDearImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


        ImGui_ImplGlfw_InitForVulkan(m_window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_Instance;
        init_info.PhysicalDevice = m_PhysicalDevice;
        init_info.Device = m_Device;
        init_info.QueueFamily = m_QueueFamily;
        init_info.Queue = m_Queue;
        init_info.PipelineCache = m_PipelineCache;
        init_info.DescriptorPool = m_DescriptorPool;
        init_info.RenderPass = m_MainWindowData.RenderPass;
        init_info.Subpass = 0;
        init_info.MinImageCount = m_MinImageCount;
        init_info.ImageCount = m_MainWindowData.ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = m_Allocator;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info);

        //initialize theme manager
        Themes::ThemeManager::Init();
    }

    void VulkanWindow::MainLoop() {
    }

    bool VulkanWindow::ShouldClose() {
        return glfwWindowShouldClose(m_window);
    }

    void VulkanWindow::Close() {
        glfwSetWindowShouldClose(m_window, true);
    }

    bool VulkanWindow::IsMinimized() {
        return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0;
    }

    bool VulkanWindow::IsMaximized() {
        return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED) != 0;
    }
}