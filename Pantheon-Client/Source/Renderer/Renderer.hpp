#pragma once

#include "Vulkan.hpp"

#include "glm/glm.hpp"
#include <filesystem>
#include <cstdint>

namespace Pantheon {

    // A RAII-friendly structure for buffer resources using Vulkan-Hpp.
    // By using vk::UniqueXXX, the destructor automatically frees resources.
    struct Buffer
    {
        vk::UniqueBuffer          handle{};
        vk::UniqueDeviceMemory    memory{};
        vk::DeviceSize            size = 0;

        // Typically usage is a bitmask of vk::BufferUsageFlagBits, so we use vk::BufferUsageFlags.
        vk::BufferUsageFlags      usage = {};
    };

    class Renderer
    {
    public:
        void Init();
        void Shutdown();

        void Render();
        void RenderCube(const glm::vec3& position);
        void RenderUI();

    private:
        void InitPipeline();
        void InitBuffers();

        void CreateOrResizeBuffer(Buffer& buffer, std::uint64_t newSize);

        // Now return a vk::UniqueShaderModule for automatic cleanup
        vk::UniqueShaderModule LoadShader(const std::filesystem::path& path);

    private:
        // Using vk::UniquePipeline and vk::UniquePipelineLayout automatically destroys
        // these resources when the Renderer goes out of scope (or on reassignment).
        vk::UniquePipeline        m_GraphicsPipeline{};
        vk::UniquePipelineLayout  m_PipelineLayout{};

        Buffer m_VertexBuffer;
        Buffer m_IndexBuffer;

        struct PushConstants
        {
            glm::mat4 ViewProjection;
            glm::mat4 Transform;
        } m_PushConstants;

        glm::vec3 m_CubePosition{ 0 };
        glm::vec3 m_CubeRotation{ 0 };

        glm::vec3 m_CameraPosition{ 0, 0, 3 };
        glm::vec3 m_CameraRotation{ 0 };
        float     m_CubeScale = 0.1f;
    };

} // namespace Pantheon
