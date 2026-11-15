#pragma once//防止重复处理头文件
#include <string>
using namespace std;
class Message
{
private:
    string content;
    string timestamp;

public:
    Message(const string& msg);
    string getContent() const;
    string getTimestamp() const;
};