# MiniEvent

一个基于CMake的C++项目。

## 项目结构

```
MiniEvent/
├── CMakeLists.txt          # CMake配置文件
├── README.md              # 项目说明
├── .gitignore            # Git忽略文件
├── include/              # 头文件目录
├── src/                  # 源代码目录
├── build/                # 构建目录
└── bin/                  # 可执行文件输出目录
```

## 构建说明

### 依赖要求

- CMake 3.16 或更高版本
- C++17 兼容的编译器 (GCC 7+, Clang 5+, MSVC 2017+)

### 构建步骤

1. 创建构建目录并进入：
```bash
mkdir build
cd build
```

2. 配置项目：
````markdown
# MiniEvent

MiniEvent 是一个轻量级的、高性能的 C++17 事件驱动网络库（受 libevent 启发），以反应器（Reactor）模式实现。目标是提供一个简单、可扩展且高效的事件循环、IO 多路复用与缓冲机制，便于构建高并发网络服务。

## 主要特性

- 基于 Reactor 模式的事件循环（`EventBase`）
- 抽象的 I/O 多路复用（`IO_Multiplexer`），支持不同平台实现
- 连接/事件封装（`Channel`, `BufferEvent`），包含高效的缓冲区 `Buffer`
- 连接监听与管理（`EVConnListener`, `ConnectionManager`）
- 采用 CMake 管理，包含单元测试（基于 GoogleTest）

## 快速使用指南

下面给出最常用的构建与运行命令（适用于 macOS / Linux，默认 shell: zsh）：

1) 创建并配置构建目录（推荐使用 Debug 类型以便调试）：

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=17 ..
```

2) 编译项目（并行构建）：

```bash
cmake --build . -- -j4
```

3) 运行示例程序（构建完成后可在 `build/bin/` 或项目 `bin/` 目录找到可执行文件）：

```bash
# 示例：运行演示程序
./build/bin/minievent_demo

# 或（如果 bin/ 下已有可执行文件）
./bin/minievent_demo
```

### 设置端口与运行小 demo

`src/demo/main.cpp` 接受一个可选的命令行参数作为监听端口，默认端口为 8080。程序启动后会打印：

```
MiniEvent demo listening on port <port>
```

示例：在 9090 端口启动 demo：

```bash
./build/bin/minievent_demo 9090
```

后台运行示例：

```bash
nohup ./build/bin/minievent_demo 9090 &
```

快速验证（注意：demo 是一个简单的 TCP 服务，不一定实现 HTTP 协议，因此推荐使用 `nc` 或 `telnet` 进行连接测试）：

```bash
# 检查端口是否可达
nc -vz 127.0.0.1 9090

# 建立交互式连接（可手动发送/接收数据）
nc 127.0.0.1 9090
```

如果你误以为是 HTTP 服务，可以用 `curl` 试探，但它可能不会返回标准 HTTP 响应：

```bash
curl -v http://127.0.0.1:9090/
```

停止服务器：在前台运行时按 Ctrl+C；若以后台方式运行，可使用 `ps`/`kill` 定位并终止进程。


4) 运行所有测试：

```bash
cd build
ctest --output-on-failure
```

5) 清理构建产物：

```bash
rm -rf build
```

## 代码布局（概要）

- `include/` — 公共头文件，库的对外接口
- `src/` — 源代码实现
- `src/demo/` — 示例代码
- `docs/` — 文档（包含 `CODE_READING_GUIDE.md`，适合初学者快速阅读代码架构）
- `tests/` — 单元/集成/冒烟测试
- `build/` — CMake 构建输出
- `bin/` — 可执行文件输出（有时为运行时样例）

更多细节请参阅 `docs/CODE_READING_GUIDE.md`，其中包含组件说明与连接生命周期等阅读指南。

## 开发与贡献

- 请在开发分支上创建 feature 分支并发起 PR。
- 本仓库内包含若干脚本（`scripts/`）用于常见开发任务，如代码检查、性能采样与冒烟测试。

## 许可证

请在项目根目录查看 LICENSE 文件（若尚未添加，请在 PR 中补充合适的开源许可证）。

````