#include <VulkanWindow.h>

#include <iostream>
#include <utility>

namespace KBUI {
    VulkanWindow::VulkanWindow(WindowProperties windowProperties) :
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
    }

    void VulkanWindow::EndFrame() {
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();

        if (!IsMinimized()) {
            m_MainWindowData.ClearValue.color.float32[0] =
                    (m_WindowProperties.getClearColor().x) * (m_WindowProperties.getClearColor().w);
            m_MainWindowData.ClearValue.color.float32[1] =
                    (m_WindowProperties.getClearColor().y) * (m_WindowProperties.getClearColor().w);
            m_MainWindowData.ClearValue.color.float32[2] =
                    (m_WindowProperties.getClearColor().z) * (m_WindowProperties.getClearColor().w);
            m_MainWindowData.ClearValue.color.float32[3] = m_WindowProperties.getClearColor().w;
            FrameRender(&m_MainWindowData, draw_data);
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

        ImGui::StyleColorsDark();
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
    }

    void VulkanWindow::MainLoop() {
        while (!ShouldClose()) {
            StartFrame();

            //example loop
            {
                if (m_showDemoWindow) {
                    ImGui::ShowDemoWindow(&m_showDemoWindow);
                    static float f = 0.0f;
                    static int counter = 0;

                    ImGui::Begin("Hello, world!");
                    ImGui::Text("This is some useful text.");
                    ImGui::Checkbox("Demo Window", &m_showDemoWindow);
                    ImGui::Checkbox("Another Window", &m_showAnotherWindow);
                    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
                    ImGui::ColorEdit3("clear color", (float *) &m_WindowProperties.getClearColorRef());

                    if (ImGui::Button("Button"))
                        counter++;
                    ImGui::SameLine();
                    ImGui::Text("counter = %d", counter);

                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                                ImGui::GetIO().Framerate);
                    ImGui::End();
                }

                if (m_showAnotherWindow) {
                    ImGui::Begin("Another Window", &m_showAnotherWindow);
                    ImGui::Text("Hello from another window!");
                    if (ImGui::Button("Close Me"))
                        m_showAnotherWindow = false;
                    ImGui::End();
                }
            }

            EndFrame();
        }
    }

    bool VulkanWindow::ShouldClose() {
        return glfwWindowShouldClose(m_window);
    }

    void VulkanWindow::Close() {
        glfwSetWindowShouldClose(m_window, true);
    }

    bool VulkanWindow::IsMinimized(){
        return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0;
    }

    bool VulkanWindow::IsMaximized(){
        return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED) != 0;
    }
}