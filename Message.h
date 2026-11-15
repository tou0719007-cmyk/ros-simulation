#pragma once//防止重复处理头文件
#include <string>
class Message
{
private:
    std::string content;
    std::string timestamp;

public:
    Message(const std::string& msg);
    std::string getContent() const;
    std::string getTimestamp() const;
};