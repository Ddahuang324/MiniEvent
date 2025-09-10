#include "../include/Buffer/BufferEvent.hpp"
#include "../include/EventBase.hpp"
#include "../include/Channel.hpp" // Ensure the full definition of Channel is included
#include <cerrno>
#include <unistd.h>


namespace MiniEvent {

BufferEvent::BufferEvent(EventBase* loop, int fd)
    : loop_(loop),
      fd_(fd),
      channel_(new Channel(loop, fd))
    {
    channel_->setEventCallback(std::bind(&BufferEvent::handleEvent, this));
    }

BufferEvent::~BufferEvent() {
  if(channel_ -> isNoneEvent()) {
      loop_->removeChannel(channel_.get());
  }
  ::close(fd_);
}


void BufferEvent::connectEstablished() {
  //连接建立后，开始监听读事件
    channel_->enableReading();
}

void BufferEvent::handleEvent(){

  int readyEvents = channel_->getReadyEvents();

  // 如果通道上有错误事件发生，则调用错误处理函数
  if (readyEvents & (Channel::kErrorEvent)) {
    handleError();
  }

  // 如果通道上有可读事件或错误事件发生，则调用读取处理函数
  if (readyEvents & (Channel::kReadEvent | Channel::kErrorEvent)) {
    handleRead();
  }

  // 如果通道上有可写事件发生，则调用写入处理函数
  if (readyEvents & Channel::kWriteEvent) {
    handleWrite();
  }

}

void BufferEvent::handleRead() {
  int savedErrno = 0;

  ssize_t n = inputBuffer_.readFd(fd_, &savedErrno);
  if(n > 0){
    if(readCallback_){
      readCallback_(shared_from_this(), &inputBuffer_);
    }
  } else if (n == 0) {
    handleClose();
  } else {
    errno = savedErrno;
    handleError();
  }
}


void BufferEvent::handleWrite() {
  // 如果 channel 正在进行写操作
  if(channel_-> isWriting()){
    ssize_t n  = ::write(fd_, outputBuffer_.peek(), outputBuffer_.readableBytes());
  // 如果写操作成功
  if(n > 0){
    outputBuffer_.retrieve(n);
    // 如果输出缓冲区已无可读取的数据
    if(outputBuffer_.readableBytes() == 0){
      channel_->disableWriting();
      // 如果有写操作完成回调函数，则调用
      if(writeCallback_){
        writeCallback_(shared_from_this());
      }
    }
  }else{
    // 处理写操作错误
    handleError();
  }
}
}

void BufferEvent::write(const void* data, size_t len) {
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    // 如果输出缓冲区为空，尝试直接发送
    if (outputBuffer_.readableBytes() == 0) {
        nwrote = ::write(fd_, data, len);
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCallback_) {
                // 全部一次性发送完毕
                writeCallback_(shared_from_this());
            }
        } else { // nwrote < 0
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                faultError = true;
            }
        }
    }

    // 如果没有出错，且还有数据没发完，就放入输出缓冲区
    if (!faultError && remaining > 0) {
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting(); // 开始监听可写事件
        }
    }
}

void BufferEvent::handleClose() {
    channel_->disableAll(); // 停止所有事件监听
    if (closeCallback_) {
        closeCallback_(shared_from_this());
    }
}

void BufferEvent::handleError() {
    channel_->disableAll(); // 停止所有事件监听
    if (errorCallback_) {
        errorCallback_(shared_from_this());
    }
}