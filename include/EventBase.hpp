#pragma once

#include "IO_Multiplexer.hpp"
#include "DataStructure/min_heap.hpp" // 需要包含最小堆
#include "DataStructure/MiniEvent_queue.hpp" // 以及我们实现的链表
#include <memory>
#include <vector>

// 前向声明
class Channel;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    
    // 添加updateChannel方法声明
    void updateChannel(Channel* channel);

private:
    // 处理到期的定时器事件
    void processExpiredTimers();
    // 获取当前时间（毫秒）
    long getCurrentTimeMs() const;
    std::unique_ptr<IOMultiplexer> io_multiplexer_;
    std::vector<Channel*> active_channels_; // 用于存放激活的 Channel
    min_heap_t timeheap_; // 用于管理定时器事件的最小堆

    bool looping_;
    bool quit_;
};