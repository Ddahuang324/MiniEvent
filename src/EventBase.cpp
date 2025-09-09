#include "../include/EventBase.hpp"
#include "../include/Platform.hpp" // 引入平台相关的宏
#include "../include/DataStructure/min_heap.hpp"
#include "../include/InheritedFromIO_Multiplexer/Epoll_multiplexer.hpp" // 引入 Epoll 实现
#include "../include/InheritedFromIO_Multiplexer/Select_multiplexer.hpp" // 引入 Select 实现
#include "../include/MiniEventLog.hpp"
#include "../include/Channel.hpp"
#include <iostream>
#include <memory>  // 添加memory头文件支持make_unique
#include <chrono>  // 添加时间相关头文件

EventLoop::EventLoop():looping_(false),quit_(false){
  min_heap_ctor(&timeheap_);  // 修正：使用min_heap_ctor而不是min_heap_elem_init

  #ifdef __linux__
    // 在 Linux 环境下，优先使用 Epoll
    io_multiplexer_.reset(new EpollMultiplexer());
    std::cout << "Using Epoll multiplexer." << std::endl;
#else
    // 在其他系统（如 macOS, Windows）下，使用通用的 Select
    io_multiplexer_.reset(new SelectMultiplexer());
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
        io_multiplexer_->dispatch(1000, active_channels_); // 修正：传递引用而不是指针

        // c. 处理所有激活的 I/O 事件
        for (Channel* channel : active_channels_) {
            // 我们不关心具体是什么事件，只管调用 Channel 自己的事件处理函数
            // Channel 会根据自身状态调用对应的回调（读、写、错误等）
            channel->handleEvent();
        }

        // d. 处理到期的定时器事件
        processExpiredTimers();
    }
    std::cout << "EventLoop " << this << " stop looping." << std::endl;
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    // TODO: 如果 loop() 正阻塞在 dispatch()，可能需要唤醒它
    // 可以考虑使用 eventfd 或 pipe 来实现唤醒机制
}

// 添加updateChannel方法实现
void EventLoop::updateChannel(Channel* channel) {
    // 委托给IO多路复用器来更新Channel
    io_multiplexer_->updateChannel(channel);
}

// 获取当前时间（毫秒）
long EventLoop::getCurrentTimeMs() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

// 处理到期的定时器事件
void EventLoop::processExpiredTimers() {
    long current_time = getCurrentTimeMs();
    
    // 检查堆顶的定时器是否到期
    while (!min_heap_empty(&timeheap_)) {
        struct event* top_event = min_heap_top(&timeheap_);
        
        // 如果堆顶的定时器还没到期，说明后面的都没到期，直接退出
        if (top_event->ev_timeout > current_time) {
            break;
        }
        
        // 弹出到期的定时器
        struct event* expired_event = min_heap_pop(&timeheap_);
        
        // TODO: 这里应该调用定时器的回调函数
        // 由于当前的event结构体还没有回调函数字段，
        // 这里只是简单地打印一条消息
        std::cout << "Timer expired at time: " << expired_event->ev_timeout << std::endl;
        
        // TODO: 在实际实现中，应该释放或重新安排定时器
        // 这里暂时不做处理，避免内存泄漏问题
    }
}