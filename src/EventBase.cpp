#include "../include/EventBase.hpp"
#include "../include/InheritedFromIO_Multiplexer/Select_multiplexer.hpp"
#include "../include/MultiplexerSelector.hpp"
#include "../include/MiniEventLog.hpp"
#include "../include/Platform.hpp"
#include "../include/ConnectionManager.hpp"
#include <assert.h>
#include <iostream>
#include <chrono>


EventBase::EventBase() :
    quit_(false)
{
    // 初始化最小堆
    min_heap_ctor(&timer_heap_);
    
    // 通过选择器基于平台与微基准选出最快实现
    io_multiplexer_ = choose_best_multiplexer();

    // 检查 IO Multiplexer 是否创建成功
    if (!io_multiplexer_) {
        log_error("Failed to create IO multiplexer.");
        abort();
    }
}


EventBase::~EventBase()
{
    // 清理最小堆
    min_heap_dtor(&timer_heap_);
}

void EventBase::loop()
{
    std::cout << "EventBase loop start." << std::endl;
    while (!quit_)
    {
        active_channels_.clear();

        // [修改] 核心修改点 1: 计算 I/O 等待超时时间
        // 根据最小堆的堆顶，计算出距离下一个定时器事件的毫秒数
        uint64_t timeoutMs = getNextTimeout();

        // 调用 I/O 多路复用等待事件
        io_multiplexer_->dispatch(static_cast<int>(timeoutMs), active_channels_);

        // 处理 I/O 事件
        for (Channel *channel : active_channels_)
        {
            channel->handleEvent();
        }

        // [修改] 核心修改点 2: 处理到期的定时器事件
        handleExpiredTimers();
    }
    std::cout << "EventBase loop stop." << std::endl;
}

void EventBase::quit()
{
    quit_ = true;
}

void EventBase::updateChannel(Channel *channel)
{
    int fd = channel->fd();
    auto it = channels_.find(fd);

    if (it == channels_.end())
    {
        // 新增 Channel - 需要先创建shared_ptr
        auto shared_channel = std::shared_ptr<Channel>(channel, [](Channel*){});
        channels_[fd] = shared_channel;
        io_multiplexer_->addChannel(channel);
    }
    else
    {
        // 更新已有 Channel
        assert(it->second.get() == channel);
        io_multiplexer_->updateChannel(channel);
    }

    // [修改] 核心修改点 3: 如果设置了超时，则添加到定时器
    if (channel->getTimeout() > 0)
    {
        addTimer(channels_[fd]);
    }
}

void EventBase::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    assert(channels_.count(fd) && channels_[fd].get() == channel);

    // [修改] 核心修改点 4: 如果在定时器中，也一并移除
    if (channel->getHeapIndex() >= 0)
    {
        removeTimer(channels_[fd]);
    }
    
    io_multiplexer_->removeChannel(channel);
    channels_.erase(fd);
}

bool EventBase::hasChannel(Channel *channel)
{
    int fd = channel->fd();
    auto it = channels_.find(fd);
    return it != channels_.end() && it->second.get() == channel;
}

void EventBase::setTimeoutObserver(std::function<void(int)> observer) {
    timeout_observer_ = std::move(observer);
}

// [新增] 获取当前时间（毫秒）
uint64_t EventBase::getCurrentTimeMs() const
{
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

// [新增] 添加/更新定时器
void EventBase::addTimer(std::shared_ptr<Channel> channel)
{
    // 创建一个event结构体来适配min_heap接口
    struct event* timer_event = new struct event;
    timer_event->ev_timeout = static_cast<long>(channel->getTimeout());
    min_heap_elem_init(timer_event);
    
    // 如果 channel 已经在堆中，则需要先移除再添加
    if (channel->getHeapIndex() >= 0)
    {
        removeTimer(channel);
    }
    
    // 将定时器添加到最小堆
    min_heap_push(&timer_heap_, timer_event);
    channel->setHeapIndex(timer_event->min_heap_idx);
}

// [新增] 移除定时器
void EventBase::removeTimer(std::shared_ptr<Channel> channel)
{
    if (channel->getHeapIndex() < 0) return;
    
    // 从最小堆中移除
    // 注意：这里需要根据实际的min_heap实现来调整
    // 当前的min_heap_erase需要event指针，这里需要适配
    channel->setHeapIndex(-1);
}

// [新增] 获取下一个超时事件的等待时间（毫秒）
uint64_t EventBase::getNextTimeout() const
{
    if (min_heap_empty(&timer_heap_))
    {
        // 如果没有定时器，可以永久等待 I/O
        return static_cast<uint64_t>(-1);
    }

    struct event* next_timeout_event = min_heap_top(&timer_heap_);
    uint64_t next_expire_time = static_cast<uint64_t>(next_timeout_event->ev_timeout);
    uint64_t now = getCurrentTimeMs();

    // 如果最近的定时器已经超时，则 dispatch 应该立即返回
    return (next_expire_time > now) ? (next_expire_time - now) : 0;
}

// [新增] 处理所有到期的定时器
void EventBase::handleExpiredTimers()
{
    uint64_t now = getCurrentTimeMs();
    expired_timer_channels_.clear();

    while (!min_heap_empty(&timer_heap_))
    {
        struct event* top_event = min_heap_top(&timer_heap_);
        if (static_cast<uint64_t>(top_event->ev_timeout) > now)
        {
            // 堆顶的定时器还未到期，结束处理
            break;
        }

        // 弹出已到期的定时器
        struct event* expired_event = min_heap_pop(&timer_heap_);

        // 找到对应的Channel并执行回调
        for (auto it = channels_.begin(); it != channels_.end(); ++it)
        {
            auto channel = it->second;
            if (channel->getTimeout() == static_cast<uint64_t>(expired_event->ev_timeout))
            {
                auto callback = channel->getTimerCallback();
                if (callback)
                {
                    // 回调（例如 HttpRequest::onTimeout）通常会记录日志或做清理
                    callback();
                }

                // 额外的全局超时处理：记录日志、统计并尝试安全关闭连接
                int fd = channel->fd();
                if (fd >= 0) {
                    // 记录超时事件
                    log_warn("Connection fd %d timed out, closing.", fd);

                    // 增加全局超时计数
                    ConnectionManager::getInstance().timeoutResponseIncrement();

                    // 如果注册了观察者，通知它（测试钩子）
                    if (timeout_observer_) {
                        try { timeout_observer_(fd); } catch (...) { }
                    }

                    // 尝试向对端发送 FIN，触发套接字状态变化
                    ::shutdown(fd, SHUT_WR);

                    // 从 EventBase 中移除该 channel（包括从 IO multiplexer 中移除）
                    // 注意：removeChannel 会处理定时器与 channel map 清理
                    // 使用当前 channel 指针进行移除
                    removeChannel(channel.get());
                } else {
                    // 对于没有 fd 的定时器（如内部 timer channel）仅重置索引
                    channel->setHeapIndex(-1);
                }

                break;
            }
        }

        // 释放event内存
        delete expired_event;
    }
}