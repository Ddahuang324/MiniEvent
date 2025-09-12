#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    const char* server_ip = "127.0.0.1"; // 服务器 IP
    const int server_port = 12345;      // 服务器端口

    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 设置服务器地址
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        ::close(sock);
        return 1;
    }

    // 连接服务器
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        ::close(sock);
        return 1;
    }

    std::cout << "Connected to server " << server_ip << ":" << server_port << std::endl;

    // 与服务器交互
    char buffer[1024];
    while (true) {
        std::cout << "Enter message: ";
        std::string message;
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        // 发送数据
        if (send(sock, message.c_str(), message.size(), 0) < 0) {
            perror("send");
            break;
        }

        // 接收回显数据
        ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            perror("recv");
            break;
        } else if (bytes_received == 0) {
            std::cout << "Server closed the connection." << std::endl;
            break;
        }

        buffer[bytes_received] = '\0'; // 确保字符串以 null 结尾
        std::cout << "Echo from server: " << buffer << std::endl;
    }

    ::close(sock);
    return 0;
}