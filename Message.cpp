#include "Message.h"
#include <chrono>//获取当前系统时间
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
using namespace std;
Message::Message(const string& msg): content(msg)
{
    if (content.empty())
    {
        throw invalid_argument("消息内容不能为空!");
    }
    auto now = chrono::system_clock::now();//获取当前时刻C++
    time_t time = chrono::system_clock::to_time_t(now);//转换成time_t类型
    static std::mutex timeMutex;
    std::lock_guard<std::mutex> lock(timeMutex);//加锁，防止多线程同时访问localtime
    stringstream ss;
    ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");//格式化输出

    timestamp = ss.str();//取时间字符串
}

const string Message::getContent() const
{
    return content;
}

const string Message::getTimestamp() const
{
    return timestamp;
}