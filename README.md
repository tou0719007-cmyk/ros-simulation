# C++ ROS Topic Communication Simulation

## 项目简介

本项目使用 C++17 模拟 ROS 的 Topic 通信机制。
不依赖真实 ROS API，通过面向对象、多线程和智能指针
实现发布者与订阅者之间的异步消息通信。

## 核心功能

1. 多发布者、多订阅者
2. Topic 消息转发
3. 消息时间戳
4. Subscriber 独立工作线程
5. 消息队列
6. 互斥锁与条件变量
7. shared_ptr 和 weak_ptr 生命周期管理
8. ROSCore 单例管理
9. 节点统一关闭

## 项目结构

```
Ros/
├── README.md
├── Message.h
├── Message.cpp
├── Node.h
├── Node.cpp
├── Publisher.h
├── Publisher.cpp
├── Subscriber.h
├── Subscriber.cpp
├── ROSCore.h
├── ROSCore.cpp
└── main.cpp
```

## 编译环境

- Windows
- MinGW-w64
- C++17

## 编译命令

```
g++ -std=c++17 -pthread main.cpp Message.cpp Node.cpp Publisher.cpp Subscriber.cpp ROSCore.cpp -o Ros.exe
```

## 运行

```
.\Ros.exe
```
