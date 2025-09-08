#include "../include/Channel.hpp"
#include "../include/EventLoop.hpp"
#include <iostream>

// --- 静态常量成员的定义 ---
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = 1;
const int Channel::kWriteEvent = 2;

// --- 构造函数 ---
Channel::Channel(EventLoop* loop, int fd){
    loop_ = loop;
    fd_ = fd;
    events_ = kNoneEvent;
    ready_events_ = kNoneEvent;
    std::cout << "Channel created" << std::endl;
}

// --- 析构函数 ---
Channel::~Channel(){}

// --- 处理事件 ---
void Channel::handleEvent(){
    //检查发生的事件，并调用相关函数.
    if(ready_events_ & kReadEvent){
        if(read_callback_ != nullptr) read_callback_();
    }
    if(ready_events_ & kWriteEvent){
        if(write_callback_ != nullptr) write_callback_();
    }
    if(ready_events_ & kErrorEvent){
        if(error_callback_ != nullptr) error_callback_();
    }
}

void Channel::enableReading(){
    events_ |= kReadEvent;
    update();
}

void Channel::enableWriting(){
    events_ |= kWriteEvent;
    update();
}

void Channel::disableWriting() {
    events_ &= ~kWriteEvent; // 使用位反和与运算，将“写”标志位清零
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}


// --- 私有辅助函数 ---
void Channel::update() {
    // 这里是关键的桥梁！
    // Channel 本身不直接与 IOMultiplexer 交互。
    // 它委托它所属的 EventLoop 去调用 IOMultiplexer 的相应方法。
    loop_->updateChannel(this);
}
