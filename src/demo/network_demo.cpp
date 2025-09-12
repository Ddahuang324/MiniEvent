#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/EventBase.hpp"
#include "../include/Buffer/BufferEvent.hpp"

using namespace std;

static int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    // 简单线程池和事件循环初始化
    EventBase loop(4);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);

    if (::bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        ::close(listen_fd);
        return 1;
    }

    if (::listen(listen_fd, 10) < 0) {
        perror("listen");
        ::close(listen_fd);
        return 1;
    }

    setNonBlocking(listen_fd);

    cout << "Echo server listening on 0.0.0.0:12345" << endl;

    // 简化：在主线程循环中轮询 accept 并创建 BufferEvent
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
    int conn_fd = ::accept(listen_fd, (sockaddr*)&client_addr, &client_len);
        if (conn_fd < 0) {
            // 非阻塞 accept 返回 -1 并设置 EAGAIN/EWOULDBLOCK
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

        setNonBlocking(conn_fd);

        // 创建 BufferEvent 管理 fd
        auto bev = std::make_shared<BufferEvent>(&loop, conn_fd);
        bev->setReadCallback([&](const BufferEvent::Ptr& bevPtr, Buffer* buf){
            // 读取到数据，回写同样内容（echo）
            std::string data(buf->peek(), buf->readableBytes());
            // 清空输入缓冲区
            buf->retrieve(buf->readableBytes());
            bevPtr->write(data.data(), data.size());
        });

        bev->setCloseCallback([&](const BufferEvent::Ptr& bevPtr){
                ::close(bevPtr->fd());
            cout << "Connection closed: " << bevPtr->fd() << endl;
        });

        bev->connectEstablished();
        cout << "Accepted connection fd=" << conn_fd << endl;
    }

    ::close(listen_fd);
    return 0;
}
