#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <sys/socket.h>

#include "../../include/ConnectionManager.hpp"

int main() {
    ConnectionManager &cm = ConnectionManager::getInstance();
    const int threads = 8;
    const int per_thread = 50;

    std::atomic<int> errors{0};
    std::vector<std::thread> ths;

    for (int t = 0; t < threads; ++t) {
        ths.emplace_back([&errors, &cm, per_thread]() {
            std::vector<int> local;
            for (int i = 0; i < per_thread; ++i) {
                int sv[2];
                if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) {
                    errors++;
                    return;
                }
                int fd = sv[0];
                cm.registerConnection(fd);
                cm.connectionIncrement();
                local.push_back(fd);
            }
            // 交替注销
            for (int fd : local) {
                cm.unregisterConnection(fd);
                cm.connectionDecrement();
                ::close(fd);
            }
        });
    }

    for (auto &t : ths) t.join();

    unsigned int reg = cm.getRegisteredConnectionsCount();
    unsigned int conns = cm.getConnections();
    std::cout << "Final registered: " << reg << ", final connections: " << conns << ", errors: " << errors << std::endl;

    return (reg == 0 && conns == 0 && errors == 0) ? 0 : 1;
}
