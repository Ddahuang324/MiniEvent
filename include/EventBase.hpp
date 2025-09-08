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

private:
    std::unique_ptr<IOMultiplexer> io_multiplexer_;
    std::vector<Channel*> active_channels_; // 用于存放激活的 Channel
    min_heap_t timeheap_; // 用于管理定时器事件的最小堆

    bool looping_;
    bool quit_;
};