# ROS 模拟通信系统测试报告

## 项目测试环境

- 操作系统：Windows
- 编译器：MinGW-w64 g++ 8.1
- 语言标准：C++17
- 构建方式：不使用 CMake，直接 g++ 编译
- 不调用真实 ROS API
- 不引入第三方测试框架

## 编译命令

核心程序编译：

```text
g++ -std=c++17 -Wall -Wextra -g main.cpp Message.cpp Node.cpp Publisher.cpp Subscriber.cpp ROSCore.cpp -o main.exe
```

测试程序编译：

```text
g++ -std=c++17 -Wall -Wextra -g tests/test_ros_sim.cpp Message.cpp Node.cpp Publisher.cpp Subscriber.cpp ROSCore.cpp -o tests/test_ros_sim.exe
```

测试运行：

```text
.\tests\test_ros_sim.exe
```

## 核心 Bug 修复记录

- 修复文件：`Subscriber.cpp`
- 修复位置：`void Subscriber::workerLoop()`
- 问题原因：
  原代码中 `Message msg("")` 在每轮循环开始时构造空消息对象，而 `Message` 构造函数会拒绝空内容并抛出 `std::invalid_argument`。
  该异常发生在 Subscriber 工作线程内部，未捕获时触发 `std::terminate`，导致程序终止。
- 修复方式：
  删除非法的空消息占位对象，改用 `std::optional<Message>` 在锁内暂存真实消息。
  只有当消息队列中确实存在消息时，才通过 `messageQueue.front()` 构造 `Message`，随后出队并在释放 `queueMutex` 后打印。
- 保留机制：
  `condition.wait()`、`stopping` 退出条件、`queueMutex` 保护、消息队列设计与线程退出逻辑均未改变。
- 提交：`fix: 修复Subscriber工作线程空消息构造问题`

## T01：基础通信测试

- 测试目的：验证一个 Publisher 是否能够向一个 Subscriber 发布消息。
- 预期行为：Subscriber 收到 Publisher 发布的消息。
- 实际结果：通过。
- 状态：PASS

## T02：一对多通信测试

- 测试目的：验证一个 Publisher 能否将消息发送给同一 Topic 下的多个 Subscriber。
- 预期行为：两个 Subscriber 都收到消息。
- 实际结果：通过。
- 状态：PASS

## T03：Topic 隔离测试

- 测试目的：验证不同 Topic 之间的消息是否相互隔离。
- 预期行为：订阅 /topic1 的 Subscriber 收到消息，订阅 /topic2 的 Subscriber 不收到消息。
- 实际结果：通过。
- 状态：PASS

## T04：连续消息测试

- 测试目的：验证连续发布多条消息时，Subscriber 是否能够正常接收。
- 预期行为：Subscriber 收到五条消息，内容正确。
- 实际结果：通过。
- 状态：PASS

## T05：异步通信测试

- 测试目的：验证 Publisher 发布消息时不需要等待 Subscriber 完成消息处理。
- 预期行为：消息能够由 Subscriber 的工作线程异步处理。
- 实际结果：通过。
- 状态：PASS

## T06：节点生命周期测试

- 测试目的：验证节点初始化、消息交互和关闭流程。
- 预期行为：init、publish、shutdown、shutdownAll 均正常执行。
- 实际结果：通过。
- 状态：PASS

## T07：空节点名称测试

- 测试目的：验证系统是否拒绝空节点名称。
- 预期行为：创建空名称节点时抛出异常。
- 实际结果：通过。
- 状态：PASS

## T08：重复节点名称测试

- 测试目的：验证重复注册节点名称时是否抛出异常。
- 预期行为：第二个同名节点 init 失败，第一个节点仍可正常使用。
- 实际结果：通过。
- 状态：PASS

## T09：空消息测试

- 测试目的：验证系统是否拒绝空消息。
- 预期行为：发布空消息时抛出异常。
- 实际结果：通过。
- 状态：PASS

## T10：不存在的 Topic 测试

- 测试目的：验证发布到无订阅者 Topic 时的行为。
- 预期行为：不抛异常，打印警告日志。
- 实际结果：通过。
- 状态：PASS

## T11：重复初始化测试

- 测试目的：验证重复调用初始化的行为。
- 预期行为：重复 init 不会崩溃，第二次直接返回。
- 实际结果：通过。
- 状态：PASS

## T12：重复关闭测试

- 测试目的：验证节点重复关闭是否会导致程序异常。
- 预期行为：重复 shutdown 不会崩溃，第二次直接返回。
- 实际结果：通过。
- 状态：PASS

## 最终测试结论

- 测试总数：12
- 通过数：12
- 失败数：0
- 所有线程正常退出，无意外 terminate。
- 无测试之间的状态污染。
- 核心代码 Bug 已修复并提交。
- 测试代码按增量阶段提交。

## 第一次运行崩溃排查记录

- 现象：第一次运行 main.exe 出现 	erminate called recursively 和 std::invalid_argument: 消息内容不能为空!，第二次运行正常。
- 根因：第一次运行的是修复前编译的 main.exe（workerLoop 中仍有 Message msg("") 占位构造），第二次运行的是修复后重新编译的 main.exe（workerLoop 已改为 std::optional<Message>）。两次运行使用了不同的编译版本，并非竞态问题。
- 验证：修复后连续运行 main.exe 20 次，全部退出码为 0，无 terminate、无 invalid_argument、无未处理异常。T01-T12 全部通过。
- 结论：当前代码无残留空消息构造问题，工作线程生命周期正常，无需进一步修改核心代码。
