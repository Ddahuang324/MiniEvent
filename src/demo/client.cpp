#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv) {
    const char* server_ip = "127.0.0.1"; // 默认服务器 IP
    int server_port = 12345;              // 默认服务器端口
    if (argc >= 2) server_ip = argv[1];
    if (argc >= 3) server_port = std::atoi(argv[2]);

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
    std::cout << "Type messages and press Enter. Type 'exit' to quit." << std::endl;

    // 与服务器交互
    char buffer[1024];
    while (true) {
        std::cout << "Enter message: ";
        std::string message;
        
        // 检查是否还有输入可读（处理管道输入）
        if (!std::getline(std::cin, message)) {
            std::cout << "No more input, closing connection." << std::endl;
            break;
        }

        if (message == "exit") {
            std::cout << "Exit command received, closing connection." << std::endl;
            break;
        }

        // 发送数据
        if (send(sock, message.c_str(), message.size(), 0) < 0) {
            perror("send");
            break;
        }
        std::cout << "[debug] sent " << message.size() << " bytes" << std::endl;

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
        std::cout << buffer << std::endl;  // 直接输出回显，不加前缀
    }    ::close(sock);
    return 0;
}