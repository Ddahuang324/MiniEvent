# data-model.md

## 实体定义

### TestEnvironment
| 字段 | 类型 | 描述 |
|------|------|------|
| os | string | 操作系统 (macOS/Linux) |
| network_mode | enum(loopback, lan, wan) | 测试网络维度 |
| multiplexer | enum(select,kqueue,epoll) | 实际使用的 IO 多路复用实现 |
| cpu_cores | int | 逻辑核数 |
| notes | string | 额外说明 |

### Component
| 字段 | 类型 | 描述 |
| name | string | 组件名称 |
| category | enum(core, io, protocol, util) | 分类 |
| critical | bool | 是否关键路径 |

### TestCase
| 字段 | 类型 | 描述 |
| id | string | 唯一 ID (TC-xxx) |
| target_component | string[] | 覆盖的组件列表 |
| type | enum(unit,component,integration,smoke) | 测试层级 |
| preconditions | string | 前置条件 |
| steps | string | 操作步骤 |
| assertions | string | 断言描述 |
| expected | string | 期望结果 |

### CoverageMatrixEntry
| 字段 | 类型 | 描述 |
| component | string | 组件 |
| unit | bool | 是否有单元测试 |
| component_test | bool | 组件级测试覆盖 |
| integration | bool | 集成覆盖 |
| smoke | bool | 冒烟覆盖 |

### Assumption
| 字段 | 类型 | 描述 |
| id | string | 标识 |
| statement | string | 内容 |
| rationale | string | 理由 |
| resolution | string | 最终处理 |

## 状态/规则
- Buffer 容量扩张: 到 1MB 上限，超过则关闭连接
- 超时清理: 连接空闲 > timeout_seconds → 回收 & 记录日志
- 日志级别过滤: 输出等级 ≥ log_level

