#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <functional>
#include <iostream>
#include <thread>
#include <atomic>

// Remove if you're not actually using spdlog. Otherwise, keep if needed.
#include "spdlog/spdlog.h"

//
// HeadlessConsole - similar to Utopia::UI::Console but for non-GUI builds
//
class HeadlessConsole
{
public:
    using MessageSendCallback = std::function<void(std::string_view)>;

public:
    explicit HeadlessConsole(std::string_view title = "Utopia Console");
    ~HeadlessConsole() noexcept;

    // Disallow copying and moving
    HeadlessConsole(const HeadlessConsole&) = delete;
    HeadlessConsole& operator=(const HeadlessConsole&) = delete;
    HeadlessConsole(HeadlessConsole&&) = delete;
    HeadlessConsole& operator=(HeadlessConsole&&) = delete;

    void ClearLog();

    template<typename... Args>
    void AddMessage(std::string_view format, Args&&... args)
    {
        std::string messageString = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
        // Replace std::cout with your logging system if desired
        std::cout << messageString << '\n';
        m_MessageHistory.emplace_back(messageString);
    }

    template<typename... Args>
    void AddItalicMessage(std::string_view format, Args&&... args)
    {
        std::string messageString = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
        MessageInfo info(messageString);
        info.Italic = true;
        m_MessageHistory.push_back(info);
        std::cout << messageString << '\n';
    }

    template<typename... Args>
    void AddTaggedMessage(std::string_view tag, std::string_view format, Args&&... args)
    {
        std::string messageString = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
        m_MessageHistory.emplace_back(std::string(tag), messageString);
        std::cout << '[' << tag << "] " << messageString << '\n';
    }

    template<typename... Args>
    void AddMessageWithColor(uint32_t color, std::string_view format, Args&&... args)
    {
        std::string messageString = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
        m_MessageHistory.emplace_back(messageString, color);
        std::cout << messageString << '\n';
    }

    template<typename... Args>
    void AddItalicMessageWithColor(uint32_t color, std::string_view format, Args&&... args)
    {
        std::string messageString = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
        MessageInfo info(messageString, color);
        info.Italic = true;
        m_MessageHistory.push_back(info);
        std::cout << messageString << '\n';
    }

    template<typename... Args>
    void AddTaggedMessageWithColor(uint32_t color, std::string_view tag, std::string_view format, Args&&... args)
    {
        std::string messageString = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
        m_MessageHistory.emplace_back(std::string(tag), messageString, color);
        std::cout << '[' << tag << "] " << messageString << '\n';
    }

    // Stub for UI rendering (headless => no actual UI)
    void OnUIRender() {}

    void SetMessageSendCallback(const MessageSendCallback& callback);

    // You can pass an atomic or condition variable to stop reading from std::cin.
    // For now, we'll rely on m_InputThreadRunning being set to false in the destructor.

private:
    void InputThreadFunc();

    struct MessageInfo
    {
        std::string Tag;
        std::string Message;
        bool Italic = false;
        uint32_t Color = 0xffffffff;

        MessageInfo(const std::string& message, uint32_t color = 0xffffffff)
            : Message(message), Color(color) {
        }

        MessageInfo(const std::string& tag, const std::string& message, uint32_t color = 0xffffffff)
            : Tag(tag), Message(message), Color(color) {
        }
    };

private:
    std::string m_Title;
    std::vector<MessageInfo> m_MessageHistory;

    std::thread m_InputThread;
    std::atomic_bool m_InputThreadRunning{ false };

    MessageSendCallback m_MessageSendCallback;
};
