#include "Node.h"

Node::Node(const std::string& name) : name(name){}

std::string Node::getName() const
{
    return name;
}