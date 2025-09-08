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
```bash
cmake ..
```

3. 编译项目：
```bash
cmake --build .
```

### 清理构建

```bash
rm -rf build
```

## 开发说明

- 将头文件放在 `include/` 目录下
- 将源文件放在 `src/` 目录下
- 可执行文件将输出到 `build/bin/` 目录
- 库文件将输出到 `build/lib/` 目录