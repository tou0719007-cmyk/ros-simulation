#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>

class Node;
class Publisher;
class Subscriber;
class Message;

class ROSCore
{
private:
    // Topic 对应的发布者
    std::map<
        std::string,
        std::vector<std::weak_ptr<Publisher>>
    > publishers;
    // Topic 对应的订阅者
    std::map<
        std::string,
        std::vector<std::weak_ptr<Subscriber>>
    > subscribers;
    // 所有已经注册的节点
    std::map<
        std::string,
        std::weak_ptr<Node>
    > nodes;
    std::mutex coreMutex;
    ROSCore() = default;
public:
    static ROSCore& getInstance();
    ROSCore(const ROSCore&) = delete;
    ROSCore& operator=(const ROSCore&) = delete;
    void registerPublisher(
        const std::string& topic,
        const std::shared_ptr<Publisher>& publisher
    );
    void registerSubscriber(
        const std::string& topic,
        const std::shared_ptr<Subscriber>& subscriber
    );
    void publish(
        const std::string& topic,
        const Message& msg
    );
    void unregisterNode(const std::string& name);
    void shutdownAll();
};
