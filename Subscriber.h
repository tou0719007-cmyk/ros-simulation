#pragma once
#include "Node.h"
#include "Message.h"
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <optional>

class Subscriber : public Node
{
private:
    std::string topic;
    std::queue<Message> messageQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::thread worker;
    bool stopping = false;
    std::atomic<bool> active{false};
    void workerLoop();
public:
    Subscriber(
        const std::string& name,
        const std::string& topic
    );
    void init() override;
    void shutdown() override;
    void receive(const Message& msg);
    ~Subscriber() override;
};
