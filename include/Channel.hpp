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
    void setReadyEvents(int ready_events) { events_ = ready_events; }

    bool isNoneEvent( ) const { return events_ == kNoneEvent; }


    void enableReading( ) ; // 启用读取功能
    void enableWriting( ) ; // 启用写入功能
    void disableWriting( ) ; // 禁用写入功能    
    void disableAll( ) ; // 禁用所有功能


    bool isWriting( ) const { return events_ & kWriteEvent; }
    bool isReading( ) const { return events_ & kReadEvent; }
    bool isError( ) const { return events_ & kErrorEvent; }

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