#include "Node.h"

Node::Node(const std::string& name) : name(name)
{
    if (name.empty())
    {
        throw std::invalid_argument("节点名称不能为空!");
    }
}

const std::string& Node::getName() const
{
    return name;
}