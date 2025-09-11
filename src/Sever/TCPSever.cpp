#include "../../include/Sever/TCPSever.hpp"
#include "../../include/MiniEventLog.hpp"


TCPSever::TCPSever(EventBase* base)
    : loop_(base), listener_(nullptr) {
    listener_ = std::make_unique<EVConnListener>(loop_, 
        std::bind(&TCPSever::onNewConnection, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

TCPSever::~TCPSever() {
    listener_.reset();
}

int TCPSever::listen(int port) {
    return listener_->listen(port);
}

MessageHandler* TCPSever::createMsgHandler() {
    // 这里可以返回一个具体的 MessageHandler 实例
    // MessageHandler 是抽象类，需要返回具体的子类实例或 nullptr
    return nullptr;
}

void TCPSever::onNewConnection(int sockfd, const struct sockaddr* addr, socklen_t len) {
    // 处理新连接
    log_info("New connection accepted, sockfd: %d", sockfd);

}

