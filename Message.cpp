#include "Message.h"
#include <chrono>//获取当前系统时间
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;
Message::Message(const string& msg): content(msg)//初始化成员列表
{
    auto now = chrono::system_clock::now();
    time_t time = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");//按指定规格输出时间%Y年%m月%d日 %H时%M分%S秒

    timestamp = ss.str();//获取格式化后的时间字符串
}

string Message::getContent() const
{
    return content;
}

string Message::getTimestamp() const
{
    return timestamp;
}