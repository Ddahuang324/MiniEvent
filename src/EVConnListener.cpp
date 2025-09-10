#include "../include/EVConnListener.hpp"
#include <unistd.h>


EVConnListener::EVConnListener(EventBase* loop, NewConnectionCallback&& new_connection_callback)
    : loop_(loop),
      new_connection_callback_(std::move(new_connection_callback)),
      listening_(false)
{
    channel_ = std::make_unique<Channel>(loop, -1);
    channel_->setReadCallback(std::bind(&EVConnListener::handleAccept, this));
}

EVConnListener::~EVConnListener()
{
    if (listening_) {
        ::close(channel_->fd());
    }
}

bool EVConnListener::listen(int port){
    // 如果已经在监听，则返回 false
    if (listening_) {
        return false; // 已经在监听
    }

    // 创建一个套接字
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return false;
    }

    // 设置套接字地址结构体
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // 设置套接字选项，允许地址重用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定套接字到指定的地址和端口
    if (::bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(sockfd);
        return false;
    }

    // 开始监听套接字
    if (::listen(sockfd, SOMAXCONN) < 0) {
        ::close(sockfd);
        return false;
    }

    // 配置事件通道以读取事件
    channel_->disableAll();
    channel_->enableReading();
    channel_->ReadyEvents(Channel::kReadEvent);
    loop_->updateChannel(channel_.get());

    // 标记为正在监听
    listening_ = true;
    return true;
}

