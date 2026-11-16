// 节点基类
#pragma once
#include <string>
#include <stdexcept>

class Node
{
protected:
    std::string name;

public:
    Node(const std::string& name);

    const std::string& getName() const;

    virtual void init() = 0;
    virtual void shutdown() = 0;

    virtual ~Node() = default;
};