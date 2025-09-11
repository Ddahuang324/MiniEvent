#include "EventBase.hpp"
#include "MultiplexerSelector.hpp"
#include <cassert>
#include <iostream>

// 测试事件循环能否正常启动和退出
void test_event_loop_start_stop() {
    MiniEvent::EventBase base;
    // 设置一个短时间的定时器，然后启动循环
    // 循环应该在定时器触发后自动退出
    base.runAfter(1, [&](){ base.stop(); });
    base.loop();
    // 如果能正常退出，说明循环是工作的
    std::cout << "test_event_loop_start_stop PASSED" << std::endl;
}

int main() {
    test_event_loop_start_stop();
    // 初始失败占位符
    // assert(false && "Event loop test placeholder failed");
    std::cout << "All Event loop tests passed (placeholders)." << std::endl;
    return 0;
}
