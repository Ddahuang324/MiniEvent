# 数据模型和合约

## 1. 数据模型

本文档主要生成的是代码阅读指南，不直接涉及持久化数据模型。涉及的数据实体主要是代码库中的 C++ 类和结构体。

### 主要数据实体 (C++ 类)

-   **`EventBase`**:
    -   `active_channels_`: `std::vector<Channel*>` - 当前事件循环中活跃的通道。
    -   `multiplexer_`: `std::unique_ptr<IO_Multiplexer>` - 使用的 IO 多路复用器。
-   **`Channel`**:
    -   `fd_`: `int` - 文件描述符。
    -   `events_`: `int` - 关注的事件类型 (读/写)。
    -   `read_callback_`, `write_callback_`, `error_callback_`: `std::function` - 事件回调。
-   **`Connection`** (由 `ConnectionManager` 管理):
    -   `channel_`: `Channel*` - 关联的事件通道。
    -   `input_buffer_`, `output_buffer_`: `Buffer` - 读写缓冲区。
-   **`Buffer`**:
    -   `buffer_`: `std::vector<char>` - 存储数据的底层容器。
    -   `read_index_`, `write_index_`: `size_t` - 读写指针。

## 2. API 合约 (Contracts)

由于我们是生成文档，因此这里的“合约”指的是我们将要文档化的核心类的公共 API 接口。

### `EventBase` API

-   `EventBase()`: 构造函数。
-   `loop()`: 启动事件循环。
-   `updateChannel(Channel* channel)`: 添加或更新一个通道。
-   `removeChannel(Channel* channel)`: 移除一个通道。

### `EVConnListener` API

-   `EVConnListener(EventBase* base, const std::string& ip, uint16_t port)`: 构造函数。
-   `setConnectionCallback(const ConnectionCallback& cb)`: 设置新连接建立时的回调。

### `BufferEvent` API

-   `BufferEvent(EventBase* base, int fd)`: 构造函数。
-   `setReadCallback(const ReadCallback& cb)`: 设置数据可读时的回调。
-   `write(const void* data, size_t len)`: 向输出缓冲区写入数据。
-   `enable(short event)`: 启用读/写事件。

## 3. 快速入门指南 (`quickstart.md`)

这份快速入门指南将作为最终文档的一部分，展示如何快速上手使用 MiniEvent 库。

### 目标

创建一个简单的回显服务器 (Echo Server)，它会监听一个端口，并将收到的任何数据原样返回给客户端。

### 步骤

1.  **包含头文件**:
    ```cpp
    #include "Sever/Sever.hpp"
    #include "MessageHandler.hpp"
    #include "Buffer/BufferEvent.hpp"
    ```

2.  **定义消息处理器**:
    创建一个继承自 `MessageHandler` 的类，并实现 `handleRead` 方法。
    ```cpp
    class EchoHandler : public MessageHandler {
    public:
        void handleRead(BufferEvent* bev) override {
            // 将输入缓冲区的数据移动到输出缓冲区
            bev->write(bev->getInputBuffer()->peek(), bev->getInputBuffer()->readableBytes());
            bev->getInputBuffer()->retrieveAll();
        }
    };
    ```

3.  **设置服务器**:
    在 `main` 函数中，创建 `Sever` 实例，设置处理器，然后启动。
    ```cpp
    int main() {
        // 1. 创建服务器实例，监听 9090 端口
        Sever server("127.0.0.1", 9090);

        // 2. 创建并设置消息处理器
        auto echo_handler = std::make_shared<EchoHandler>();
        server.setMessageHandler(echo_handler);

        // 3. 启动服务器
        server.start();

        return 0;
    }
    ```

4.  **编译和运行**:
    -   使用 CMake 构建项目。
    -   运行生成的可执行文件。
    -   使用 `telnet` 或 `netcat` 连接到 `127.0.0.1:9090` 进行测试。

这个快速入门指南将为新用户提供一个清晰、可操作的起点。
