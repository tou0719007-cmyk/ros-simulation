#pragma once
#include <string>
#include <memory>
#include <stdexcept>

class Node : public std::enable_shared_from_this<Node>
{
protected:
    std::string name;
public:
    explicit Node(const std::string& name);
    const std::string& getName() const;
    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual ~Node() = default;
};
