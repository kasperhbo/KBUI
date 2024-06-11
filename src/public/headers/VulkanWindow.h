//
// Created by Kasper de bruin on 10/06/2024.
//
#ifndef KBUI_VULKANWINDOW_H
#define KBUI_VULKANWINDOW_H

#include <ImVecMath.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <Properties/WindowProperties.h>

namespace KBUI {

    class VulkanWindow {
    public:
        explicit VulkanWindow(WindowProperties  windowProperties);

        ~VulkanWindow();

        bool Initialize();

        void StartFrame();

        void EndFrame();

        bool ShouldClose();
        bool IsMinimized();
        bool IsMaximized();

        void Close();

    private:
        static bool InitializeGLFW();

        bool InitializeVulkan();

        void SetupDearImGui();

        void MainLoop();

        static bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension) {
            for (const VkExtensionProperties &p: properties)
                if (strcmp(p.extensionName, extension) == 0)
                    return true;
            return false;
        }

        static void glfw_error_callback(int error, const char *description) {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        }

        static void check_vk_result(VkResult err) {
            if (err == 0)
                return;
            fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
            if (err < 0)
                abort();
        }

        VkPhysicalDevice SetupVulkan_SelectPhysicalDevice() {
            uint32_t gpu_count;
            VkResult err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, nullptr);
            check_vk_result(err);
            IM_ASSERT(gpu_count > 0);

            ImVector<VkPhysicalDevice> gpus;
            gpus.resize(gpu_count);
            err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, gpus.Data);
            check_vk_result(err);

            // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
            // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
            // dedicated GPUs) is out of scope of this sample.
            for (VkPhysicalDevice &device: gpus) {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(device, &properties);
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                    return device;
            }

            // Use first GPU (Integrated) is a Discrete one is not available.
            if (gpu_count > 0)
                return gpus[0];
            return VK_NULL_HANDLE;
        }

        void SetupVulkan(ImVector<const char *> instance_extensions) {
            VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
            volkInitialize();
#endif

            // Create Vulkan Instance
            {
                VkInstanceCreateInfo create_info = {};
                create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

                // Enumerate available extensions
                uint32_t properties_count;
                ImVector<VkExtensionProperties> properties;
                vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
                properties.resize(properties_count);
                err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.Data);
                check_vk_result(err);

                // Enable required extensions
                if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
                    instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
                if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
                    instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
                }
#endif

                // Enabling validation layers
#ifdef APP_USE_VULKAN_DEBUm_REPORT
                const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = layers;
        instance_extensions.push_back("VK_EXT_debum_report");
#endif

                // Create Vulkan Instance
                create_info.enabledExtensionCount = (uint32_t) instance_extensions.Size;
                create_info.ppEnabledExtensionNames = instance_extensions.Data;
                err = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
                check_vk_result(err);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
                volkLoadInstance(m_Instance);
#endif

                // Setup the debug report callback
#ifdef APP_USE_VULKAN_DEBUm_REPORT
                auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugReportCallbackEXT");
        IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);
        VkDebugReportCallbackCreateInfoEXT debum_report_ci = {};
        debum_report_ci.sType = VK_STRUCTURE_TYPE_DEBUm_REPORT_CALLBACK_CREATE_INFO_EXT;
        debum_report_ci.flags = VK_DEBUm_REPORT_ERROR_BIT_EXT | VK_DEBUm_REPORT_WARNINm_BIT_EXT | VK_DEBUm_REPORT_PERFORMANCE_WARNINm_BIT_EXT;
        debum_report_ci.pfnCallback = debum_report;
        debum_report_ci.pUserData = nullptr;
        err = f_vkCreateDebugReportCallbackEXT(m_Instance, &debum_report_ci, m_Allocator, &m_DebugReport);
        check_vk_result(err);
#endif
            }

            // Select Physical Device (GPU)
            m_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();

            // Select graphics queue family
            {
                uint32_t count;
                vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
                VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *) malloc(
                        sizeof(VkQueueFamilyProperties) * count);
                vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queues);
                for (uint32_t i = 0; i < count; i++)
                    if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        m_QueueFamily = i;
                        break;
                    }
                free(queues);
                IM_ASSERT(m_QueueFamily != (uint32_t) -1);
            }

            // Create Logical Device (with 1 queue)
            {
                ImVector<const char *> device_extensions;
                device_extensions.push_back("VK_KHR_swapchain");

                // Enumerate physical device extension
                uint32_t properties_count;
                ImVector<VkExtensionProperties> properties;
                vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &properties_count, nullptr);
                properties.resize(properties_count);
                vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
                if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

                const float queue_priority[] = {1.0f};
                VkDeviceQueueCreateInfo queue_info[1] = {};
                queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_info[0].queueFamilyIndex = m_QueueFamily;
                queue_info[0].queueCount = 1;
                queue_info[0].pQueuePriorities = queue_priority;
                VkDeviceCreateInfo create_info = {};
                create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
                create_info.pQueueCreateInfos = queue_info;
                create_info.enabledExtensionCount = (uint32_t) device_extensions.Size;
                create_info.ppEnabledExtensionNames = device_extensions.Data;
                err = vkCreateDevice(m_PhysicalDevice, &create_info, m_Allocator, &m_Device);
                check_vk_result(err);
                vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);
            }

            // Create Descriptor Pool
            // The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
            // If you wish to load e.g. additional textures you may need to alter pools sizes.
            {
                VkDescriptorPoolSize pool_sizes[] =
                        {
                                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
                        };
                VkDescriptorPoolCreateInfo pool_info = {};
                pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                pool_info.maxSets = 1;
                pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
                pool_info.pPoolSizes = pool_sizes;
                err = vkCreateDescriptorPool(m_Device, &pool_info, m_Allocator, &m_DescriptorPool);
                check_vk_result(err);
            }
        }

        void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height) {
            wd->Surface = surface;

            // Check for WSI support
            VkBool32 res;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_QueueFamily, wd->Surface, &res);
            if (res != VK_TRUE) {
                fprintf(stderr, "Error no WSI support on physical device 0\n");
                exit(-1);
            }

            // Select Surface Format
            const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
                                                          VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
            const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
            wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_PhysicalDevice, wd->Surface,
                                                                      requestSurfaceImageFormat,
                                                                      (size_t) IM_ARRAYSIZE(requestSurfaceImageFormat),
                                                                      requestSurfaceColorSpace);

            // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
            VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
            VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
            wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_PhysicalDevice, wd->Surface, &present_modes[0],
                                                                  IM_ARRAYSIZE(present_modes));
            //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

            // Create SwapChain, RenderPass, Framebuffer, etc.
            IM_ASSERT(m_MinImageCount >= 2);
            ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device, wd, m_QueueFamily,
                                                   m_Allocator, width, height, m_MinImageCount);
        }

        void CleanupVulkan() {
            vkDestroyDescriptorPool(m_Device, m_DescriptorPool, m_Allocator);

#ifdef APP_USE_VULKAN_DEBUm_REPORT
            // Remove the debug report callback
    auto f_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");
    f_vkDestroyDebugReportCallbackEXT(m_Instance, m_DebugReport, m_Allocator);
#endif // APP_USE_VULKAN_DEBUm_REPORT

            vkDestroyDevice(m_Device, m_Allocator);
            vkDestroyInstance(m_Instance, m_Allocator);
        }

        void CleanupVulkanWindow() {
            ImGui_ImplVulkanH_DestroyWindow(m_Instance, m_Device, &m_MainWindowData, m_Allocator);
        }

        void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data) {
            VkResult err;

            VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
            VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
            err = vkAcquireNextImageKHR(m_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE,
                                        &wd->FrameIndex);
            if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
                m_SwapChainRebuild = true;
                return;
            }
            check_vk_result(err);

            ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
            {
                err = vkWaitForFences(m_Device, 1, &fd->Fence, VK_TRUE,
                                      UINT64_MAX);    // wait indefinitely instead of periodically checking
                check_vk_result(err);

                err = vkResetFences(m_Device, 1, &fd->Fence);
                check_vk_result(err);
            }
            {
                err = vkResetCommandPool(m_Device, fd->CommandPool, 0);
                check_vk_result(err);
                VkCommandBufferBeginInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
                check_vk_result(err);
            }
            {
                VkRenderPassBeginInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.renderPass = wd->RenderPass;
                info.framebuffer = fd->Framebuffer;
                info.renderArea.extent.width = wd->Width;
                info.renderArea.extent.height = wd->Height;
                info.clearValueCount = 1;
                info.pClearValues = &wd->ClearValue;
                vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
            }

            // Record dear imgui primitives into command buffer
            ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

            // Submit command buffer
            vkCmdEndRenderPass(fd->CommandBuffer);
            {
                VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                VkSubmitInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                info.waitSemaphoreCount = 1;
                info.pWaitSemaphores = &image_acquired_semaphore;
                info.pWaitDstStageMask = &wait_stage;
                info.commandBufferCount = 1;
                info.pCommandBuffers = &fd->CommandBuffer;
                info.signalSemaphoreCount = 1;
                info.pSignalSemaphores = &render_complete_semaphore;

                err = vkEndCommandBuffer(fd->CommandBuffer);
                check_vk_result(err);
                err = vkQueueSubmit(m_Queue, 1, &info, fd->Fence);
                check_vk_result(err);
            }
        }

        void FramePresent(ImGui_ImplVulkanH_Window *wd) {
            if (m_SwapChainRebuild)
                return;
            VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
            VkPresentInfoKHR info = {};
            info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &render_complete_semaphore;
            info.swapchainCount = 1;
            info.pSwapchains = &wd->Swapchain;
            info.pImageIndices = &wd->FrameIndex;
            VkResult err = vkQueuePresentKHR(m_Queue, &info);
            if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
                m_SwapChainRebuild = true;
                return;
            }
            check_vk_result(err);
            wd->SemaphoreIndex =
                    (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
        }

    private:
        // Vulkan variables
        VkInstance m_Instance{};
        VkPhysicalDevice m_PhysicalDevice{};
        VkDevice m_Device{};
        uint32_t m_QueueFamily{};
        VkQueue m_Queue{};
        VkPipelineCache m_PipelineCache{};
        VkDescriptorPool m_DescriptorPool{};
        VkAllocationCallbacks *m_Allocator;
        ImGui_ImplVulkanH_Window m_MainWindowData;
        bool m_SwapChainRebuild;
        uint32_t m_MinImageCount;

        // Window variables
        GLFWwindow *m_window = nullptr;

        bool m_showDemoWindow = true;
        bool m_showAnotherWindow = false;
        WindowProperties m_WindowProperties;
    };

} // KBUI

#endif //KBUI_VULKANWINDOW_H
