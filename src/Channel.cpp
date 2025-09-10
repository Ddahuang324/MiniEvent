#include "../include/Channel.hpp"
#include "../include/EventBase.hpp"
#include "../include/MiniEventLog.hpp"

namespace MiniEvent {
// --- 静态常量成员的定义 ---
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = 1;
const int Channel::kWriteEvent = 2;
const int Channel::kErrorEvent = 4;

// --- 构造函数 ---
Channel::Channel(MiniEvent::EventBase* loop, int fd)
    : loop_(loop), fd_(fd), events_(0), ready_events_(0), 
      timeout_(0), heap_index_(-1) {
}

// --- 析构函数 ---
Channel::~Channel(){}

// --- 处理事件 ---
void Channel::handleEvent(){
    // 优先调用统一事件回调
    // 否则按事件类型分别回调
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
    events_ &= ~kWriteEvent; // 使用位反和与运算，将"写"标志位清零
    update();
}

void Channel::disableReading() {
    events_ &= ~kReadEvent; // 使用位反和与运算，将"读"标志位清零
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}

// --- 私有辅助函数 ---
void Channel::update() {
    loop_->updateChannel(this);
}

}
