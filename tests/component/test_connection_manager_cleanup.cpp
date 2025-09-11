#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>

#include "../../include/ConnectionManager.hpp"

int main() {
    ConnectionManager &cm = ConnectionManager::getInstance();

    // 创建一批 socketpair，并注册 fd
    std::vector<int> fds;
    for (int i = 0; i < 10; ++i) {
        int sv[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) {
            std::cerr << "socketpair failed" << std::endl;
            return 2;
        }
        fds.push_back(sv[0]);
        cm.registerConnection(sv[0]);
    }

    unsigned int reg = cm.getRegisteredConnectionsCount();
    std::cout << "Registered count: " << reg << std::endl;
    if (reg != 10) return 3;

    // 注销并关闭
    for (int fd : fds) {
        cm.unregisterConnection(fd);
        ::close(fd);
    }

    unsigned int reg2 = cm.getRegisteredConnectionsCount();
    std::cout << "Registered after cleanup: " << reg2 << std::endl;
    return (reg2 == 0) ? 0 : 4;
}
