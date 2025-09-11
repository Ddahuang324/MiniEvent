# MiniEvent 架构图表

## HttpContainer 应用架构

```mermaid
flowchart TD
    subgraph "🏗️ HttpContainer (应用顶层)"
        direction TB
        subgraph "抽象服务器层"
            AS1["🌐 AbstractServer<br/>TcpServer"]
            AS2["🌐 AbstractServer<br/>HttpServer"]
        end
        
        HC["🎯 HttpContainer<br/>容器管理器"]
        HC -.->|"管理"| AS1
        HC -.->|"管理"| AS2
    end

    subgraph "🎪 业务逻辑层 [Message Processing]"
        direction TB
        MH["📡 MessageHandler<br/>(接口)"]
        
        subgraph "具体实现"
            direction LR
            TSMH["📊 TcpServerMsgHandler<br/>TCP消息处理"]
            HSMH["📋 HttpServerMsgHandler<br/>HTTP消息处理"]
        end
    end

    subgraph "🔄 数据流与交互"
        direction TB
        REQ["📨 请求消息"]
        RESP["📤 响应消息"]
    end

    %% 主要数据流
    AS1 ==>|"消息处理"| MH
    AS2 ==>|"消息处理"| MH
    
    %% 接口实现关系
    MH -.->|"接口实现"| TSMH
    MH -.->|"接口实现"| HSMH
    
    %% 消息流向
    REQ -->|"传入"| AS1
    REQ -->|"传入"| AS2
    TSMH -->|"处理后"| RESP
    HSMH -->|"处理后"| RESP

    %% 样式定义
    style MH fill:#e3f2fd,stroke:#1565c0,stroke-width:4px
    style TSMH fill:#e8f5e8,stroke:#2e7d32,stroke-width:2px
    style HSMH fill:#e8f5e8,stroke:#2e7d32,stroke-width:2px
    style HC fill:#f3e5f5,stroke:#7b1fa2,stroke-width:3px
    style AS1 fill:#e0f2f1,stroke:#00695c,stroke-width:2px
    style AS2 fill:#e0f2f1,stroke:#00695c,stroke-width:2px
    
    %% 定义样式类
    classDef interface fill:#e3f2fd,stroke:#1565c0,stroke-width:4px
    classDef implementation fill:#e8f5e8,stroke:#2e7d32,stroke-width:2px
    classDef container fill:#f3e5f5,stroke:#7b1fa2,stroke-width:3px
    classDef server fill:#e0f2f1,stroke:#00695c,stroke-width:2px
    classDef dataFlow stroke:#2196f3,stroke-width:3px
    classDef signalFlow stroke:#f44336,stroke-width:2px,stroke-dasharray: 5 5
```

## 架构说明

### 🎯 核心设计理念
- **分层架构**: 清晰的应用层、业务层分离
- **接口抽象**: MessageHandler提供统一的消息处理接口
- **多态实现**: 支持不同协议的消息处理器
- **容器管理**: HttpContainer统一管理服务器实例

### 📊 组件职责

#### 应用顶层 (HttpContainer)
- **HttpContainer**: 容器管理器，负责协调各个服务器实例
- **AbstractServer**: 抽象服务器基类，提供TCP和HTTP服务器的通用功能

#### 业务逻辑层 (Message Processing)
- **MessageHandler**: 消息处理接口，定义标准的消息处理契约
- **TcpServerMsgHandler**: TCP协议消息处理的具体实现
- **HttpServerMsgHandler**: HTTP协议消息处理的具体实现

### 🔄 数据流向
1. **请求接收**: 外部请求通过AbstractServer接收
2. **消息处理**: 通过MessageHandler接口进行统一处理
3. **具体实现**: 根据协议类型调用相应的处理器
4. **响应返回**: 处理结果通过相同路径返回

### 🎨 设计优势
- **可扩展性**: 新增协议只需实现MessageHandler接口
- **可维护性**: 清晰的职责分离和接口抽象
- **可测试性**: 接口抽象便于单元测试和模拟测试
- **灵活性**: 支持运行时切换不同的消息处理实现
