#include "Message.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>

Message::Message(const std::string& msg)
    : content(msg)
{
    if (msg.empty())
    {
        throw std::invalid_argument("消息内容不能为空");
    }
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime =
        std::chrono::system_clock::to_time_t(now);
    static std::mutex timeMutex;
    std::lock_guard<std::mutex> lock(timeMutex);
    std::stringstream ss;
    ss << std::put_time(
        std::localtime(&currentTime),
        "%Y-%m-%d %H:%M:%S"
    );
    timestamp = ss.str();
}

const std::string& Message::getContent() const
{
    return content;
}

const std::string& Message::getTimestamp() const
{
    return timestamp;
}
