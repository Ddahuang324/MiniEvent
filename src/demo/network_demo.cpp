// Refactored echo server using EVConnListener + multi-thread business processing.
#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/EventBase.hpp"
#include "../include/Buffer/BufferEvent.hpp"
#include "../include/EVConnListener.hpp"

using namespace std;

int main(int argc, char** argv) {
    int port = 12345; if (argc > 1) port = std::atoi(argv[1]);
    EventBase loop(4); // worker threads for business processing
    std::mutex conn_mtx;
    std::vector<std::shared_ptr<BufferEvent>> connections;

    EVConnListener listener(&loop, [&](int sockfd, const struct sockaddr*, socklen_t){
        auto bev = std::make_shared<BufferEvent>(&loop, sockfd);
        {
            std::lock_guard<std::mutex> lk(conn_mtx);
            connections.push_back(bev);
        }
        bev->setReadCallback([&loop](const BufferEvent::Ptr& bevPtr, Buffer* buf){
            size_t n = buf->readableBytes();
            std::string data(buf->peek(), n);
            buf->retrieve(n);
            std::cout << "[READ] fd=" << bevPtr->fd() << " bytes=" << n << " data='" << data << "'" << std::endl;
            auto weak = std::weak_ptr<BufferEvent>(bevPtr);
            std::cout << "[READ] dispatching to worker thread" << std::endl;
            loop.executeInWorker([weak, &loop, data]{
                std::cout << "[WORKER] processing data, queuing write-back" << std::endl;
                loop.runInLoop([weak, data]{
                    if (auto locked = weak.lock()) {
                        std::cout << "[WRITE-BACK] writing " << data.size() << " bytes to fd=" << locked->fd() << std::endl;
                        locked->write(data.data(), data.size());
                    } else {
                        std::cout << "[WRITE-BACK] connection already closed" << std::endl;
                    }
                });
            });
        });
        bev->setCloseCallback([&](const BufferEvent::Ptr& bevPtr){
            std::cout << "[CLOSE] Connection closing for fd=" << bevPtr->fd() << std::endl;
            std::lock_guard<std::mutex> lk(conn_mtx);
            auto old_size = connections.size();
            connections.erase(std::remove_if(connections.begin(), connections.end(), [&](auto &p){ return p.get()==bevPtr.get(); }), connections.end());
            ::close(bevPtr->fd());
            std::cout << "[CLOSE] Connection closed fd=" << bevPtr->fd() << " (active connections: " << old_size << " -> " << connections.size() << ")" << std::endl;
        });
        bev->setErrorCallback([&](const BufferEvent::Ptr& bevPtr){
            std::cout << "Connection error fd=" << bevPtr->fd() << std::endl;
        });
        bev->connectEstablished();
        size_t active; { std::lock_guard<std::mutex> lk(conn_mtx); active = connections.size(); }
        std::cout << "Accepted connection fd=" << sockfd << " (active=" << active << ")" << std::endl;
    });

    if (listener.listen(port) != 0) {
        std::cerr << "listen failed on port " << port << std::endl; return 1;
    }
    std::cout << "Echo server listening on 0.0.0.0:" << port << std::endl;
    loop.loop();
    return 0;
}
