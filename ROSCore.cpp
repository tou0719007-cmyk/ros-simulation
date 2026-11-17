#include "ROSCore.h"

#include "Node.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "Message.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>

ROSCore& ROSCore::getInstance()
{
    static ROSCore instance;
    return instance;
}

void ROSCore::registerNode(const std::shared_ptr<Node>& node)
{
    std::lock_guard<std::mutex> lock(coreMutex);

    const std::string& name = node->getName();

    auto it = nodes.find(name);

    if (it != nodes.end() && !it->second.expired())
    {
        throw std::runtime_error("节点名称重复: " + name);
    }

    nodes[name] = node;
}

void ROSCore::registerPublisher(
    const std::string& topic,
    const std::shared_ptr<Publisher>& publisher
)
{
    registerNode(publisher);

    std::lock_guard<std::mutex> lock(coreMutex);

    publishers[topic].push_back(publisher);
}

void ROSCore::registerSubscriber(
    const std::string& topic,
    const std::shared_ptr<Subscriber>& subscriber
)
{
    registerNode(subscriber);

    std::lock_guard<std::mutex> lock(coreMutex);

    subscribers[topic].push_back(subscriber);
}

void ROSCore::publish(
    const std::string& topic,
    const Message& msg
)
{
    std::vector<std::shared_ptr<Subscriber>> targets;

    {
        std::lock_guard<std::mutex> lock(coreMutex);

        auto it = subscribers.find(topic);

        if (it == subscribers.end())
        {
            std::cout << "[WARN] Topic 没有订阅者: "
                      << topic << std::endl;
            return;
        }

        auto& list = it->second;

        for (auto iter = list.begin(); iter != list.end();)
        {
            if (auto subscriber = iter->lock())
            {
                targets.push_back(subscriber);
                ++iter;
            }
            else
            {
                iter = list.erase(iter);
            }
        }
    }

    for (const auto& subscriber : targets)
    {
        subscriber->receive(msg);
    }
}

void ROSCore::unregisterNode(const std::string& name)
{
    std::lock_guard<std::mutex> lock(coreMutex);

    nodes.erase(name);

    for (auto& item : publishers)
    {
        auto& list = item.second;

        list.erase(
            std::remove_if(
                list.begin(),
                list.end(),
                [&name](const std::weak_ptr<Publisher>& weak)
                {
                    auto ptr = weak.lock();
                    return !ptr || ptr->getName() == name;
                }
            ),
            list.end()
        );
    }

    for (auto& item : subscribers)
    {
        auto& list = item.second;

        list.erase(
            std::remove_if(
                list.begin(),
                list.end(),
                [&name](const std::weak_ptr<Subscriber>& weak)
                {
                    auto ptr = weak.lock();
                    return !ptr || ptr->getName() == name;
                }
            ),
            list.end()
        );
    }
}

void ROSCore::shutdownAll()
{
    std::vector<std::shared_ptr<Node>> activeNodes;

    {
        std::lock_guard<std::mutex> lock(coreMutex);

        for (auto& item : nodes)
        {
            if (auto node = item.second.lock())
            {
                activeNodes.push_back(node);
            }
        }
    }

    for (const auto& node : activeNodes)
    {
        node->shutdown();
    }

    std::cout << "[INFO] 所有节点已关闭" << std::endl;
}