#pragma once

#include <functional>
#include <memory>


class EventLoop;



class Channel {
public:
    using EventCallback = std::function<void()>;

    //事件掩码常量
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    static const int kErrorEvent;


    Channel(EventLoop* loop, int fd);
    ~Channel();
    
    //核心：当事件发生时，有EventLoop调用
    void handleEvent();

     // 设置不同事件的回调函数
    void setReadCallback(EventCallback cb) { read_callback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { write_callback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { error_callback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { close_callback_ = std::move(cb); }

    int fd( )const { return fd_; }
    int events( )const { return events_; }
    
    // 应该是：
    void ReadyEvents(int ready_events) { ready_events_ = ready_events; }

    bool isNoneEvent( ) const { return events_ == kNoneEvent; }

//注册事件用的
    void enableReading( ) ; // 告诉系统监听fd上的读事件
    void enableWriting( ) ; // 告诉系统开始监听这个fd上的写事件
    void disableWriting( ) ; //停止监听写事件
    void disableAll( ) ; // 停止监听所有事件


    bool isWriting( ) const { return events_ & kWriteEvent; }//检查当前是否启用了写事件监听
    bool isReading( ) const { return events_ & kReadEvent; }//检查当前是否启用了读事件监听
    bool isError( ) const { return events_ & kErrorEvent; }//检查当前是否启用了错误事件监听

    EventLoop* ownerLoop( ) const { return loop_; }

private:
    void update(); // 更新事件掩码
    
    EventLoop* loop_;
    const int fd_;

    int events_;

    int ready_events_; // 已经准备好的事件掩码

     // 回调函数
    EventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback error_callback_;
    EventCallback close_callback_; // 用于连接关闭的回调

};