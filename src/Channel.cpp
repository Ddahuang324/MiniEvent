#include "../include/Channel.hpp"
#include "../include/EventBase.hpp"
#include <iostream>

// --- 静态常量成员的定义 ---
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = 1;
const int Channel::kWriteEvent = 2;
const int Channel::kErrorEvent = 4;

// --- 构造函数 ---
Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop), fd_(fd), events_(kNoneEvent), ready_events_(kNoneEvent),
      timeout_(0),     // [新增] 初始化超时时间为 0
      heap_index_(-1)  // [新增] 初始化堆索引为 -1
{
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
