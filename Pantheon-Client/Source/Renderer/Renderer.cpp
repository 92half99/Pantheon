#include "Renderer.hpp"

#include "Utopia/Application.hpp"
#include "Utopia/Core/Log.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <imgui.h>
#include <array>
#include <fstream>

namespace Pantheon {

    static inline vk::CommandBuffer toCppCommandBuffer(VkCommandBuffer cmd)
    {
        return vk::CommandBuffer(cmd);
    }

    static uint32_t ImGui_ImplVulkan_MemoryType(vk::PhysicalDevice physicalDevice,
        vk::MemoryPropertyFlags properties,
        uint32_t type_bits)
    {
        vk::PhysicalDeviceMemoryProperties memProps = physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            const bool isRequiredMemType = (type_bits & (1 << i)) != 0;
            const bool hasProperties = (memProps.memoryTypes[i].propertyFlags & properties) == properties;

            if (isRequiredMemType && hasProperties)
                return i;
        }
        return 0xFFFFFFFF; // Unable to find suitable memory type
    }

    void Renderer::Init()
    {
        InitBuffers();
        InitPipeline();
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::Render()
    {
        VkCommandBuffer rawCmdBuffer = Utopia::Application::GetActiveCommandBuffer();
        vk::CommandBuffer cmdBuffer = toCppCommandBuffer(rawCmdBuffer);

        auto wd = Utopia::Application::GetMainWindowData();
        float viewportW = static_cast<float>(wd->Width);
        float viewportH = static_cast<float>(wd->Height);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_GraphicsPipeline.get());

        glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 3));

        m_PushConstants.ViewProjection = glm::perspectiveFov(glm::radians(45.0f), viewportW, viewportH, 0.1f, 1000.0f)
            * glm::inverse(cameraTransform);

        m_PushConstants.Transform = glm::translate(glm::mat4(1.0f), m_CubePosition)
            * glm::eulerAngleXYZ(glm::radians(m_CubeRotation.x),
                glm::radians(m_CubeRotation.y),
                glm::radians(m_CubeRotation.z));

        cmdBuffer.pushConstants(
            m_PipelineLayout.get(),
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(PushConstants),
            &m_PushConstants
        );

        vk::Buffer     vertexBuf = m_VertexBuffer.handle.get();
        vk::DeviceSize vertexOffset = 0;

        cmdBuffer.bindVertexBuffers(0, { vertexBuf }, { vertexOffset });

        vk::Buffer indexBuf = m_IndexBuffer.handle.get();
        cmdBuffer.bindIndexBuffer(indexBuf, 0, vk::IndexType::eUint32);

        // Set viewport
        cmdBuffer.setViewport(
            0,
            { vk::Viewport{
                0.0f,           // x
                0.0f,           // y
                viewportW,      // width
                viewportH,      // height
                0.0f,           // minDepth
                1.0f            // maxDepth
            } }
        );

        // Set scissor
        cmdBuffer.setScissor(
            0,
            { vk::Rect2D{
                vk::Offset2D{0, 0},
                vk::Extent2D{
                    static_cast<uint32_t>(wd->Width),
                    static_cast<uint32_t>(wd->Height)
                }
            } }
        );

        cmdBuffer.drawIndexed(36, 1, 0, 0, 0);
    }

    void Renderer::RenderCube(const glm::vec3& position)
    {
        VkCommandBuffer rawCmdBuffer = Utopia::Application::GetActiveCommandBuffer();
        vk::CommandBuffer cmdBuffer = toCppCommandBuffer(rawCmdBuffer);

        auto wd = Utopia::Application::GetMainWindowData();
        float viewportW = static_cast<float>(wd->Width);
        float viewportH = static_cast<float>(wd->Height);

        // Bind pipeline
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_GraphicsPipeline.get());

        // Camera transform
        glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), m_CameraPosition)
            * glm::eulerAngleXYZ(glm::radians(m_CameraRotation.x),
                glm::radians(m_CameraRotation.y),
                glm::radians(m_CameraRotation.z));

        m_PushConstants.ViewProjection = glm::perspectiveFov(glm::radians(45.0f), viewportW, viewportH, 0.1f, 1000.0f)
            * glm::inverse(cameraTransform);

        glm::vec3 translation = position * m_CubeScale;
        m_PushConstants.Transform = glm::translate(glm::mat4(1.0f), translation)
            * glm::eulerAngleXYZ(glm::radians(m_CubeRotation.x),
                glm::radians(m_CubeRotation.y),
                glm::radians(m_CubeRotation.z));

        cmdBuffer.pushConstants(
            m_PipelineLayout.get(),
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(PushConstants),
            &m_PushConstants
        );

        // Bind vertex/index buffers
        cmdBuffer.bindVertexBuffers(0, { m_VertexBuffer.handle.get() }, { 0 });
        cmdBuffer.bindIndexBuffer(m_IndexBuffer.handle.get(), 0, vk::IndexType::eUint32);

        // Set viewport/scissor
        cmdBuffer.setViewport(
            0,
            { vk::Viewport{
                0.0f, 0.0f, viewportW, viewportH, 0.0f, 1.0f
            } }
        );

        cmdBuffer.setScissor(
            0,
            { vk::Rect2D{
                  vk::Offset2D{ 0, 0 },
                  vk::Extent2D{
                      static_cast<uint32_t>(wd->Width),
                      static_cast<uint32_t>(wd->Height)
                  }
              } }
        );

        // Draw
        cmdBuffer.drawIndexed(36, 1, 0, 0, 0);
    }

    void Renderer::RenderUI()
    {
        ImGui::Begin("Controls");

        ImGui::DragFloat3("Position", glm::value_ptr(m_CameraPosition), 0.05f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(m_CameraRotation), 0.05f);
        ImGui::DragFloat("Scale", &m_CubeScale, 0.0005f);

        ImGui::End();
    }

    void Renderer::InitPipeline()
    {
        vk::Device     device = vk::Device(GetVulkanInfo()->Device);
        vk::RenderPass renderPass = vk::RenderPass(Utopia::Application::GetMainWindowData()->RenderPass);

        vk::PushConstantRange pushConstantRange(
            vk::ShaderStageFlagBits::eVertex, // stageFlags
            0,                                // offset
            sizeof(glm::mat4) * 2             // size
        );

        vk::PipelineLayoutCreateInfo layoutInfo;
        layoutInfo.setPushConstantRanges(pushConstantRange);

        m_PipelineLayout = device.createPipelineLayoutUnique(layoutInfo);

        // Vertex binding/attribute description
        vk::VertexInputBindingDescription bindingDesc(
            0,                             // binding
            sizeof(glm::vec3),             // stride
            vk::VertexInputRate::eVertex   // inputRate
        );

        vk::VertexInputAttributeDescription attributeDesc(
            0,                              // location
            0,                              // binding
            vk::Format::eR32G32B32Sfloat,   // format
            0                               // offset
        );

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo(
            {},
            1, &bindingDesc,
            1, &attributeDesc
        );

        // Input assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
            {},
            vk::PrimitiveTopology::eTriangleList
        );

        // Rasterization
        vk::PipelineRasterizationStateCreateInfo rasterState(
            {},
            false,                               // depthClampEnable
            false,                               // rasterizerDiscardEnable
            vk::PolygonMode::eFill,             // polygonMode
            vk::CullModeFlagBits::eBack,        // cullMode
            vk::FrontFace::eClockwise,          // frontFace
            false                               // depthBiasEnable
        );
        rasterState.setLineWidth(1.0f);

        // Color blend
        vk::PipelineColorBlendAttachmentState blendAttachment(
            false,                                       // blendEnable
            vk::BlendFactor::eOne, vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlags(
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA
            )
        );

        vk::PipelineColorBlendStateCreateInfo colorBlendState(
            {},
            false,                           // logicOpEnable
            vk::LogicOp::eClear,            // logicOp
            1, &blendAttachment
        );

        // Viewport & scissor will be dynamic
        vk::PipelineViewportStateCreateInfo viewportState(
            {},
            1, nullptr,
            1, nullptr
        );

        // Depth/stencil OFF
        vk::PipelineDepthStencilStateCreateInfo depthStencil;

        // Multisample OFF
        vk::PipelineMultisampleStateCreateInfo multisample;
        multisample.setRasterizationSamples(vk::SampleCountFlagBits::e1);

        // Dynamic states
        std::array<vk::DynamicState, 2> dynamics{
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineDynamicStateCreateInfo dynamicState({}, dynamics.size(), dynamics.data());

        auto vertShaderModule = LoadShader("Assets/Shaders/bin/basic.vert.spirv");
        auto fragShaderModule = LoadShader("Assets/Shaders/bin/basic.frag.spirv");

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {
            vk::PipelineShaderStageCreateInfo(
                {},
                vk::ShaderStageFlagBits::eVertex,
                vertShaderModule.get(),
                "main"
            ),
            vk::PipelineShaderStageCreateInfo(
                {},
                vk::ShaderStageFlagBits::eFragment,
                fragShaderModule.get(),
                "main"
            )
        };

        // Finally create the pipeline
        vk::GraphicsPipelineCreateInfo pipelineInfo(
            {},
            static_cast<uint32_t>(shaderStages.size()),
            shaderStages.data(),
            &vertexInputInfo,
            &inputAssembly,
            nullptr,
            &viewportState,
            &rasterState,
            &multisample,
            &depthStencil,
            &colorBlendState,
            &dynamicState,
            m_PipelineLayout.get(),
            renderPass
        );

        auto pipelineResult = device.createGraphicsPipelineUnique({}, pipelineInfo);
        if (pipelineResult.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
        m_GraphicsPipeline = std::move(pipelineResult.value);
    }

    void Renderer::InitBuffers()
    {
        vk::Device device = vk::Device(GetVulkanInfo()->Device);

        glm::vec3 vertexData[8] = {
            {-0.5f, -0.5f,  0.5f},
            {-0.5f,  0.5f,  0.5f},
            { 0.5f,  0.5f,  0.5f},
            { 0.5f, -0.5f,  0.5f},

            { 0.5f, -0.5f, -0.5f},
            { 0.5f,  0.5f, -0.5f},
            {-0.5f,  0.5f, -0.5f},
            {-0.5f, -0.5f, -0.5f}
        };

        uint32_t indices[36] = {
            0,1,2,  2,3,0,  3,2,5,  5,4,3,
            4,5,6,  6,7,4,  7,6,1,  1,0,7,
            1,6,5,  5,2,1,  7,0,3,  3,4,7
        };

        m_VertexBuffer.usage = vk::BufferUsageFlagBits::eVertexBuffer;
        CreateOrResizeBuffer(m_VertexBuffer, sizeof(vertexData));

        m_IndexBuffer.usage = vk::BufferUsageFlagBits::eIndexBuffer;
        CreateOrResizeBuffer(m_IndexBuffer, sizeof(indices));

        void* vbMemory = device.mapMemory(m_VertexBuffer.memory.get(), 0, sizeof(vertexData));
        std::memcpy(vbMemory, vertexData, sizeof(vertexData));
        device.flushMappedMemoryRanges({ vk::MappedMemoryRange(m_VertexBuffer.memory.get(), 0, VK_WHOLE_SIZE) });
        device.unmapMemory(m_VertexBuffer.memory.get());

        void* ibMemory = device.mapMemory(m_IndexBuffer.memory.get(), 0, sizeof(indices));
        std::memcpy(ibMemory, indices, sizeof(indices));
        device.flushMappedMemoryRanges({ vk::MappedMemoryRange(m_IndexBuffer.memory.get(), 0, VK_WHOLE_SIZE) });
        device.unmapMemory(m_IndexBuffer.memory.get());
    }

    vk::UniqueShaderModule Renderer::LoadShader(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            UT_ERROR("Could not open shader file: {}", path.string());
            return {};
        }

        file.seekg(0, std::ios::end);
        std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size))
        {
            UT_ERROR("Could not read shader file: {}", path.string());
            return {};
        }
        file.close();

        vk::ShaderModuleCreateInfo createInfo(
            {},
            buffer.size(),
            reinterpret_cast<const uint32_t*>(buffer.data())
        );

        vk::Device device = vk::Device(GetVulkanInfo()->Device);

        return device.createShaderModuleUnique(createInfo);
    }

    void Renderer::CreateOrResizeBuffer(Buffer& buffer, std::uint64_t newSize)
    {
        vk::Device       device = vk::Device(GetVulkanInfo()->Device);
        vk::PhysicalDevice physicalDevice = vk::PhysicalDevice(GetVulkanInfo()->PhysicalDevice);

        vk::BufferCreateInfo bufInfo(
            {},
            newSize,
            buffer.usage,
            vk::SharingMode::eExclusive
        );

        auto newBuffer = device.createBufferUnique(bufInfo);

        vk::MemoryRequirements memReqs = device.getBufferMemoryRequirements(newBuffer.get());

        vk::MemoryAllocateInfo allocInfo(
            memReqs.size,
            ImGui_ImplVulkan_MemoryType(physicalDevice,
                vk::MemoryPropertyFlagBits::eHostVisible,
                memReqs.memoryTypeBits)
        );
        auto newMemory = device.allocateMemoryUnique(allocInfo);

        device.bindBufferMemory(newBuffer.get(), newMemory.get(), 0);

        buffer.handle = std::move(newBuffer);
        buffer.memory = std::move(newMemory);
        buffer.size = memReqs.size;
    }

} // namespace Pantheon
