#include "Publisher.h"
#include "ROSCore.h"

#include <iostream>
#include <stdexcept>


Publisher::Publisher(const std::string& name,const std::string& topic) : Node(name), topic(topic){}

void Publisher::init()
{
    if (active.load())
    {
        return;
    }

    auto self = std::dynamic_pointer_cast<Publisher>(shared_from_this());

    ROSCore::getInstance().registerPublisher(topic, self);

    active.store(true);

    std::cout << "[INFO] Publisher " << name
              << " 初始化成功,Topic: " << topic << std::endl;
}

void Publisher::publish(const std::string& content)
{
    if (!active.load())
    {
        throw std::runtime_error("发布者尚未初始化");
    }

    Message msg(content);

    std::cout << "[PUBLISH] " << name
              << " 发布消息: " << msg.getContent() << std::endl;

    ROSCore::getInstance().publish(topic, msg);
}

void Publisher::shutdown()
{
    if (!active.exchange(false))
    {
        return;
    }

    ROSCore::getInstance().unregisterNode(name);

    std::cout << "[INFO] Publisher " << name
              << " 已关闭" << std::endl;
}

Publisher::~Publisher()
{
    shutdown();
}