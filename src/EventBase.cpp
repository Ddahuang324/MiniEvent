#include "../include/EventBase.hpp"
#include "../include/Platform.hpp" // 引入平台相关的宏
#include "../include/DataStructure/min_heap.hpp"
#include "../include/InheritedFromIO_Multiplexer/Epoll_multiplexer.hpp" // 引入 Epoll 实现
#include "../include/InheritedFromIO_Multiplexer/Select_multiplexer.hpp" // 引入 Select 实现
#include "../include/MiniEventLog.hpp"
#include "../include/Channel.hpp"
#include <iostream>

EventLoop::EventLoop():looping_(false),quit_(false){
  min_heap_elem_init(&timeheap_);

  #ifdef __linux__
    // 在 Linux 环境下，优先使用 Epoll
    io_multiplexer_ = std::make_unique<EpollMultiplexer>();
    std::cout << "Using Epoll multiplexer." << std::endl;
#else
    // 在其他系统（如 macOS, Windows）下，使用通用的 Select
    io_multiplexer_ = std::make_unique<SelectMultiplexer>();
    std::cout << "Using Select multiplexer." << std::endl;
#endif

    // 3. 检查 IO Multiplexer 是否创建成功
    if (!io_multiplexer_) {
        // 在实际项目中，这里应该记录严重错误日志并可能导致程序中止
        log_error("Failed to create IO multiplexer.");
        abort();
    }
}

// 析构函数
EventLoop::~EventLoop() {
    // `io_multiplexer_` 是 std::unique_ptr，
    // 它会在 EventLoop 对象销毁时自动调用所管理对象的析构函数，
    // 也就是会自动 delete 指向的 Epoll_multiplexer 或 Select_multiplexer 实例。
    // 这就是 RAII 的魔力，我们不需要在这里写任何清理代码。

    // 我们需要手动清理最小堆
    min_heap_dtor(&timeheap_);
}

void EventLoop::loop() {   
    looping_ = true;
    quit_ = false;

    std::cout << "EventLoop::loop() start." << std::endl;


    while (!quit_){
        // a. 在每一轮循环开始前，清空上一轮的激活 Channel 列表
        active_channels_.clear();

        // b. 调用 IO Multiplexer 等待事件，这是事件循环的核心
        // dispatch 会阻塞，直到有事件发生或超时
        // 激活的 Channel 会被填充到 active_channels_ 中
        io_multiplexer_->dispatch(1000, &active_channels_); // 暂时硬编码超时1秒

        // c. 处理所有激活的 I/O 事件
        for (Channel* channel : active_channels_) {
            // 我们不关心具体是什么事件，只管调用 Channel 自己的事件处理函数
            // Channel 会根据自身状态调用对应的回调（读、写、错误等）
            channel->handleEvent();
        }

        // d. (TODO) 在这里处理到期的定时器事

    }
    std::cout << "EventLoop " << this << " stop looping." << std::endl;
    looping_ = false;
}
void EventLoop::quit() {
    quit_ = true;
    // (TODO) 如果 loop() 正阻塞在 dispatch()，可能需要唤醒它
}