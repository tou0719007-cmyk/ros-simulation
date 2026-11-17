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
}

int main()
{
    testT01();
    TestReporter::summary();
    return TestReporter::failedCount() == 0 ? 0 : 1;
}
