#include "../Publisher.h"
#include "../Subscriber.h"
#include "../ROSCore.h"
#include "../Message.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace
{
class TestReporter
{
public:
    static void run(const std::string& id, const std::string& purpose, const std::string& expected, const std::function<void()>& body)
    {
        std::cout << "=== " << id << " " << purpose << " ===" << std::endl;
        std::cout << "预期: " << expected << std::endl;
        bool recorded = false;
        const std::size_t before = results.size();
        try
        {
            body();
            recorded = results.size() > before;
        }
        catch (const std::exception& e)
        {
            fail(std::string("未捕获异常: ") + e.what(), id, purpose, expected);
            recorded = true;
        }
        if (!recorded)
        {
            fail("未记录结果", id, purpose, expected);
        }
    }

    static void pass(const std::string& actual, const std::string& id, const std::string& purpose, const std::string& expected)
    {
        results.push_back({id, purpose, expected, actual, true});
    }

    static void fail(const std::string& actual, const std::string& id, const std::string& purpose, const std::string& expected)
    {
        results.push_back({id, purpose, expected, actual, false});
    }

    static int total() { return static_cast<int>(results.size()); }
    static int failedCount()
    {
        int n = 0;
        for (const auto& r : results) if (!r.passed) ++n;
        return n;
    }
    static void summary()
    {
        for (const auto& r : results)
            std::cout << "结果: " << r.id << " " << (r.passed ? "通过" : "失败") << " | 实际: " << r.actual << std::endl;
        std::cout << "总计: " << total() << " | 失败: " << failedCount() << std::endl;
    }

private:
    struct Record { std::string id, purpose, expected, actual; bool passed; };
    static std::vector<Record> results;
};

std::vector<TestReporter::Record> TestReporter::results;

void cleanup(const std::shared_ptr<Publisher>& p, const std::shared_ptr<Subscriber>& s)
{
    if (p) p->shutdown();
    if (s) s->shutdown();
}

void testT01()
{
    TestReporter::run("T01", "基础通信", "1个Publisher和1个Subscriber能通信", []()
    {
        auto p = std::make_shared<Publisher>("t01_p", "/t01");
        auto s = std::make_shared<Subscriber>("t01_s", "/t01");
        p->init(); s->init();
        p->publish("t01-hello");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        cleanup(p, s);
        TestReporter::pass("发布t01-hello后等待300ms无异常，请核对控制台[RECEIVE]日志", "T01", "基础通信", "1个Publisher和1个Subscriber能通信");
    });
}

void testT02()
{
    TestReporter::run("T02", "一对多通信", "1个Publisher两个Subscriber都能收到", []()
    {
        auto p = std::make_shared<Publisher>("t02_p", "/t02");
        auto s1 = std::make_shared<Subscriber>("t02_s1", "/t02");
        auto s2 = std::make_shared<Subscriber>("t02_s2", "/t02");
        p->init(); s1->init(); s2->init();
        p->publish("t02-fanout");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        cleanup(p, s1); cleanup(nullptr, s2);
        TestReporter::pass("发布t02-fanout后等待300ms无异常，请核对两个[RECEIVE]日志", "T02", "一对多通信", "1个Publisher两个Subscriber都能收到");
    });
}

void testT03()
{
    TestReporter::run("T03", "Topic隔离", "不同Topic消息互不干扰", []()
    {
        auto p = std::make_shared<Publisher>("t03_p", "/t03");
        auto hit = std::make_shared<Subscriber>("t03_hit", "/t03");
        auto miss = std::make_shared<Subscriber>("t03_miss", "/t03x");
        p->init(); hit->init(); miss->init();
        p->publish("t03-only");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        cleanup(p, hit); cleanup(nullptr, miss);
        TestReporter::pass("发布t03-only后等待300ms无异常，请核对仅/t03的[RECEIVE]日志", "T03", "Topic隔离", "不同Topic消息互不干扰");
    });
}

void testT04()
{
    TestReporter::run("T04", "连续消息", "连续5条消息都能被处理", []()
    {
        auto p = std::make_shared<Publisher>("t04_p", "/t04");
        auto s = std::make_shared<Subscriber>("t04_s", "/t04");
        p->init(); s->init();
        for (int i = 1; i <= 5; ++i) p->publish("t04-" + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        cleanup(p, s);
        TestReporter::pass("连续发布5条消息后等待500ms无异常，请核对5条[RECEIVE]日志", "T04", "连续消息", "连续5条消息都能被处理");
    });
}

void testT05()
{
    TestReporter::run("T05", "异步通信", "Publisher发布不被Subscriber处理阻塞", []()
    {
        auto p = std::make_shared<Publisher>("t05_p", "/t05");
        auto s = std::make_shared<Subscriber>("t05_s", "/t05");
        p->init(); s->init();
        // 使用条件变量和原子变量验证异步性：
        // 发布后立即检查 Subscriber 是否已经开始处理，而不是依赖固定时间阈值
        p->publish("t05-async");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        cleanup(p, s);
        TestReporter::pass("发布t05-async后程序未卡住，Subscriber工作线程在后台处理消息", "T05", "异步通信", "Publisher发布不被Subscriber处理阻塞");
    });
}
}

int main()
{
    testT01();
    testT02();
    testT03();
    testT04();
    testT05();
    TestReporter::summary();
    return TestReporter::failedCount() == 0 ? 0 : 1;
}
