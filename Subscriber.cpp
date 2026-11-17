#include "Subscriber.h"
#include "ROSCore.h"

#include <iostream>
#include <stdexcept>
#include <optional>

Subscriber::Subscriber(
    const std::string& name,
    const std::string& topic
) : Node(name), topic(topic)
{
    if (topic.empty())
    {
        throw std::invalid_argument("Topic 名称不能为空");
    }
}

void Subscriber::init()
{
    if (active.load())
    {
        return;
    }

    auto self = std::dynamic_pointer_cast<Subscriber>(
        shared_from_this()
    );

    ROSCore::getInstance().registerSubscriber(topic, self);

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopping = false;
    }

    active.store(true);

    worker = std::thread(&Subscriber::workerLoop, this);

    std::cout << "[INFO] Subscriber " << name
              << " 初始化成功，Topic: " << topic << std::endl;
}

void Subscriber::receive(const Message& msg)
{
    std::lock_guard<std::mutex> lock(queueMutex);

    if (stopping || !active.load())
    {
        return;
    }

    messageQueue.push(msg);

    condition.notify_one();
}

void Subscriber::workerLoop()
{
    while (true)
    {
        std::optional<Message> msg;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this]()
            {
                return stopping || !messageQueue.empty();
            });

            if (stopping && messageQueue.empty())
            {
                break;
            }

            msg = std::make_optional<Message>(messageQueue.front());
            messageQueue.pop();
        }

        if (msg)
        {
            std::cout << "[RECEIVE] " << name
                      << " 收到消息: " << msg->getContent()
                      << " | 时间: " << msg->getTimestamp()
                      << std::endl;
        }
    }
}

void Subscriber::shutdown()
{
    if (!active.exchange(false))
    {
        return;
    }

    ROSCore::getInstance().unregisterNode(name);

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopping = true;
    }

    condition.notify_all();

    if (worker.joinable())
    {
        worker.join();
    }

    std::cout << "[INFO] Subscriber " << name
              << " 已关闭" << std::endl;
}

Subscriber::~Subscriber()
{
    shutdown();
}
