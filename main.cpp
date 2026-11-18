#include "Publisher.h"
#include "Subscriber.h"
#include "ROSCore.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <cstdlib>

int main()
{
    system("chcp 65001");
    try
    {
        // 创建发布者
        auto pub1 = std::make_shared<Publisher>(
            "publisher1", "/chatter"
        );
        auto pub2 = std::make_shared<Publisher>(
            "publisher2", "/chatter"
        );
        // 创建订阅者
        auto sub1 = std::make_shared<Subscriber>(
            "subscriber1", "/chatter"
        );
        auto sub2 = std::make_shared<Subscriber>(
            "subscriber2", "/chatter"
        );
        // 初始化节点
        pub1->init();
        pub2->init();
        sub1->init();
        sub2->init();
        // 发布消息
        pub1->publish("Hello ROS!");
        pub2->publish("C++ Topic Communication");
        pub1->publish("第二条消息");
        // 等待订阅者处理消息
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
        // 统一关闭所有节点
        ROSCore::getInstance().shutdownAll();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        ROSCore::getInstance().shutdownAll();
        return 1;
    }
    return 0;
}
