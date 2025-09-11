#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/EventBase.hpp"
#include "../include/Buffer/BufferEvent.hpp"
#include "../include/ConnectionManager.hpp"
#include "../include/Sever/TimeOutServer.hpp"
#include "../include/MiniEventLog.hpp"

using namespace std::chrono_literals;

int main() {
    // 初始化 EventBase
    EventBase base;

    // 启动内部定时器服务（非必须，但保持行为一致）
    TimeoutServer timeoutSrv(&base);
    timeoutSrv.listen(0);

    // 创建 socketpair 模拟客户端/服务端的一端
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) {
        std::cerr << "socketpair failed" << std::endl;
        return 1;
    }

    // 直接创建 Channel 并注册到 EventBase，设置一个短超时（绝对时间 ms）
    auto ch = new Channel(&base, sv[0]);
    ch->setTimerCallback([fd = sv[0]](){
        log_warn("Test channel fd %d timed out", fd);
        ConnectionManager::getInstance().timeoutResponseIncrement();
        ::shutdown(fd, SHUT_WR);
    });

    // 计算绝对超时（使用 steady_clock）
    auto now = std::chrono::steady_clock::now();
    uint64_t now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    ch->setTimeout(now_ms + 500);

    // 注册 channel 到 EventBase
    base.updateChannel(ch);

    // 在后台启动事件循环
    std::thread loop_thread([&base]{ base.loop(); });

    // 等待 2 秒以便超时触发
    std::this_thread::sleep_for(2s);

    // 检查超时计数是否递增
    long long timeouts = ConnectionManager::getInstance().getTimeoutResponse();
    std::cout << "Timeouts observed: " << timeouts << std::endl;

    // 清理
    ::close(sv[0]);
    ::close(sv[1]);

    // 停止 event loop
    base.quit();
    loop_thread.join();

    return (timeouts > 0) ? 0 : 1;
}
