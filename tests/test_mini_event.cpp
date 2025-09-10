#include "../include/EventBase.hpp"
#include "../include/Channel.hpp"
#include "../include/EVConnListener.hpp"
#include "../include/Buffer/BufferEvent.hpp"
#include "../include/Platform.hpp"
#include <cassert>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Tiny helpers
static int create_client_socket(const char* ip, int port) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    ::inet_pton(AF_INET, ip, &addr.sin_addr);
    if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(fd);
        return -1;
    }
    return fd;
}

int main() {
    std::cout << "[Tests] MiniEvent functional tests start" << std::endl;

    // 1) Buffer basic
    {
        Buffer buf;
        const char* msg = "hello";
        buf.append(msg, std::strlen(msg));
        assert(buf.readableBytes() == 5);
        std::string s = buf.retrieveAllAsString();
        assert(s == "hello");
    }

    // 2) Timer via EventBase/Channel
    {
        EventBase loop;
        std::atomic<bool> fired{false};
        // 使用虚拟fd=-1的Channel仅用于计时器
        Channel timerChan(&loop, -1);
        timerChan.setTimerCallback([&]{ fired = true; loop.quit(); });
        // 设定 200ms 后超时
        uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        timerChan.setTimeout(now + 200);
        loop.updateChannel(&timerChan);
        std::thread t([&]{ loop.loop(); });
        t.join();
        assert(fired.load());
    }

    // 3) EVConnListener + BufferEvent echo
    {
        EventBase loop;
        std::atomic<bool> server_ready{false};
        std::atomic<bool> got_echo{false};
        const int port = 18080; // test port

        EVConnListener listener(&loop, [&](int sockfd, const struct sockaddr* addr, socklen_t len){
            (void)addr; (void)len;
            auto bev = std::make_shared<BufferEvent>(&loop, sockfd);
            bev->setReadCallback([&](const BufferEvent::Ptr& b, Buffer* input){
                std::string data = input->retrieveAllAsString();
                // echo back
                b->write(data.data(), data.size());
                got_echo = true;
                loop.quit();
            });
            bev->connectEstablished();
        });
        bool ok = listener.listen(port);
        assert(ok);

        std::thread server([&]{
            server_ready = true;
            loop.loop();
        });

        // wait server
        while(!server_ready.load()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        int cfd = create_client_socket("127.0.0.1", port);
        assert(cfd >= 0);
        const char* payload = "ping";
        ::send(cfd, payload, std::strlen(payload), 0);
        // read echo
        char buf[16] = {0};
        int n = ::recv(cfd, buf, sizeof(buf), 0);
        // Some environments may need more time; but our server quits on first echo.
        // If n<=0 immediately, give a small wait and try again once.
        if (n <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            n = ::recv(cfd, buf, sizeof(buf), 0);
        }
        ::close(cfd);

        server.join();
        assert(n >= 4);
        assert(std::string(buf, 4) == "ping");
        assert(got_echo.load());
    }

    std::cout << "[Tests] All assertions passed." << std::endl;
    return 0;
}
