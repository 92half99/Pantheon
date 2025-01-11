#include "HeadlessConsole.hpp"

HeadlessConsole::HeadlessConsole(std::string_view title)
    : m_Title(title)
{
    // NOTE: To run in background on a Linux server you'll need to comment out
    //       the following line, since we can't std::getline with no terminal
    m_InputThread = std::thread([this]()
        {
            InputThreadFunc();
        });
}

HeadlessConsole::~HeadlessConsole() noexcept
{
    // Signal the input thread to stop
    m_InputThreadRunning = false;
    if (m_InputThread.joinable())
    {
        m_InputThread.join();
    }
}

void HeadlessConsole::ClearLog()
{
    m_MessageHistory.clear();
}

void HeadlessConsole::SetMessageSendCallback(const MessageSendCallback& callback)
{
    m_MessageSendCallback = callback;
}

void HeadlessConsole::InputThreadFunc()
{
    // Mark the thread as running
    m_InputThreadRunning = true;

    while (m_InputThreadRunning.load())
    {
        std::string line;
        // If std::getline fails (e.g., EOF or no valid console), break out
        if (!std::getline(std::cin, line))
        {
            // Sleep or yield to avoid tight loop if no console
            // For a headless build, you may want to handle this differently
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // If a callback is set, pass the line
        if (m_MessageSendCallback)
        {
            m_MessageSendCallback(line);
        }
    }
}
