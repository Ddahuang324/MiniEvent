#pragma once

#include "IO_Multiplexer.hpp"
#include "Channel.hpp"
#include "DataStructure/min_heap.hpp"
#include "Platform.hpp"
#include <memory>
#include <vector>
#include <map>
#include <cstdint>

namespace MiniEvent
{

// [新增] 为最小堆定义比较器
// 比较两个 Channel 的绝对超时时间
struct ChannelTimeoutComparator
{
    bool operator()(const std::shared_ptr<Channel> &a, const std::shared_ptr<Channel> &b) const
    {
        return a->getTimeout() > b->getTimeout();
    }
};

class EventBase
{
public:
    EventBase();
    ~EventBase();

    // 启动事件循环
    void loop();
    // 退出事件循环
    void quit();

    // 更新/添加 Channel
    void updateChannel(Channel *channel);
    // 移除 Channel
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

private:
    // [新增] 定时器相关私有方法
    void addTimer(std::shared_ptr<Channel> channel);
    void removeTimer(std::shared_ptr<Channel> channel);
    void handleExpiredTimers();
    uint64_t getNextTimeout() const;
    uint64_t getCurrentTimeMs() const;

    bool quit_;
    std::unique_ptr<IOMultiplexer> io_multiplexer_;
    std::map<int, std::shared_ptr<Channel>> channels_;
    std::vector<Channel *> active_channels_;

    // [新增] 定时器管理数据结构
    min_heap_t timer_heap_;
    std::vector<std::shared_ptr<Channel>> expired_timer_channels_;
};

} // namespace MiniEvent