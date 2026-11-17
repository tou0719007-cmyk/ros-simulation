#pragma once

#include "Node.h"
#include "Message.h"

#include <string>
#include <atomic>

class Publisher : public Node
{
private:
    std::string topic;
    std::atomic<bool> active{false};//支持多线程

public:
    Publisher(const std::string& name,const std::string& topic);

    void init() override;
    void shutdown() override;

    void publish(const std::string& content);

    ~Publisher() override;
};