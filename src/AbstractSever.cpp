#include "../include/AbstractSever.hpp"

AbstractServer::AbstractServer(void)
    : msgHandler_(nullptr) {
}

AbstractServer::~AbstractServer(void) {
    delete msgHandler_;
}


void AbstractServer::run(struct EventBase* base ) {
    if( !msgHandler_ ) {
        msgHandler_ = createMsgHandler();
    }
    listen(base);
}

MessageHandler* AbstractServer::createMsgHandler() {
    // 默认实现返回空指针，派生类应覆盖此方法以提供具体的消息处理器实例
    return nullptr;
}

int AbstractServer::listen(struct EventBase* base) {
    // 默认实现返回 -1，派生类应覆盖此方法以实现具体的监听逻辑
    return -1;
}
