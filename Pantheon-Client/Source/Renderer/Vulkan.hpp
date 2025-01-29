#pragma once

#include <vulkan/vulkan.hpp>
#include "backends/imgui_impl_vulkan.h"
#include <string>
#include <iostream>

namespace vkb {
    std::string to_string(VkResult result) noexcept;

}

namespace Pantheon {
    ImGui_ImplVulkan_InitInfo* GetVulkanInfo() noexcept;
}

inline void VK_CHECK(VkResult result) noexcept
{
    if (result != VK_SUCCESS)
    {
        std::cerr << "Vulkan Error: " << vkb::to_string(result) << '\n';
    }
}
