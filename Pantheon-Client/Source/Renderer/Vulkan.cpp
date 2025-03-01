#include "Vulkan.hpp"

namespace vkb {

    std::string to_string(VkResult result) noexcept
    {
        switch (result)
        {
        case VK_NOT_READY:
            return "NOT_READY";
        case VK_TIMEOUT:
            return "TIMEOUT";
        case VK_EVENT_SET:
            return "EVENT_SET";
        case VK_EVENT_RESET:
            return "EVENT_RESET";
        case VK_INCOMPLETE:
            return "INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:
            return "ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:
            return "ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            return "ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED:
            return "PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "ERROR_INVALID_SHADER_NV";
        default:
            return "UNKNOWN_ERROR";
        }
    }

} // namespace vkb

namespace Pantheon {

    ImGui_ImplVulkan_InitInfo* GetVulkanInfo() noexcept
    {
        if (ImGui::GetCurrentContext())
        {
            return static_cast<ImGui_ImplVulkan_InitInfo*>(ImGui::GetIO().BackendRendererUserData);
        }
        return nullptr;
    }

} // namespace Pantheon
