#pragma once
#include <string>
class Message
{
private:
    std::string content;
    std::string timestamp;
public:
    explicit Message(const std::string& msg);
    const std::string& getContent() const;
    const std::string& getTimestamp() const;
};
