#pragma once//防止重复处理头文件
#include <string>
class Message
{
private:
    std::string content;
    std::string timestamp;

public:
    explicit Message(const std::string& msg);
    const std::string getContent() const;
    const std::string getTimestamp() const;
};
