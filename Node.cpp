#include "Node.h"

Node::Node(const string& name): name(name){}

string Node::getName() const
{
    return name;
}