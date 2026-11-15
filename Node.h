//节点基类
#pragma once
#include <string>
using namespace std;
class Node
{
protected:
    string name;//节点名称

public:
    Node(const string& name);//构造函数

    string getName() const;//获取节点名称

    virtual void init() = 0;//纯虚函数
    virtual void shutdown() = 0;

    virtual ~Node() = default;//析构函数
};