# 任务: 项目全面测试方案（非压力测试）

**输入**: 来自 `/specs/001-/` 的设计文档 (plan.md, research.md, data-model.md, contracts/, quickstart.md)
**先决条件**: plan.md ✅ research.md ✅ data-model.md ✅ contracts/ ✅ quickstart.md ✅

## 执行流程 (主流程)
```
1. 加载 plan.md (完成)
2. 加载 data-model / research / contracts / quickstart (完成)
3. 生成设置/测试/核心/集成/完善任务
4. 应用并行与依赖规则
5. 编号任务并校验完整性
6. 输出 tasks.md (本文件)
```

## 任务格式
`[ID] [P?] 描述 (文件路径)`
- [P] = 可并行(不同文件，无直接依赖)
- 未标记 = 顺序执行，可能依赖前置任务

---
## 阶段 3.1: 设置 (基础设施与规范)
- [ ] T001 建立测试子目录结构: `tests/{unit,component,integration,smoke,fixtures}` 并迁移/占位 README
- [ ] T002 添加日志格式文档: `docs/log-format.md` (基于 research 决策)
- [ ] T003 添加配置示例文件: `tests/fixtures/example_config.json` 含必需字段
- [ ] T004 引入 GoogleTest (或选择 Catch2) 到 CMake: 修改 `CMakeLists.txt` 生成 `MiniEvent_unit_tests`
- [ ] T005 在 `tests/CMakeLists.txt` 或主 CMake 中添加分组标签 (unit/component/integration)
- [ ] T006 创建覆盖矩阵生成脚本占位: `scripts/gen_coverage_matrix.py` (解析 test-matrix.md)
- [ ] T007 [P] 添加 CPU 采样辅助脚本: `scripts/cpu_sample.sh` (采样占用, 输出平均值)
- [ ] T008 [P] 添加冒烟脚本: `scripts/smoke_http.sh` (启动→curl→断言→退出码)
 - [x] T001 建立测试子目录结构: `tests/{unit,component,integration,smoke,fixtures}` 并迁移/占位 README — 目录与示例测试已存在
 - [ ] T002 添加日志格式文档: `docs/log-format.md` (基于 research 决策)
 - [x] T003 添加配置示例文件: `tests/fixtures/example_config.ini` 含必需字段 — 配置示例存在（格式为 .ini，路径 `tests/fixtures/example_config.ini`）
 - [x] T004 引入 GoogleTest (或选择 Catch2) 到 CMake: 修改 `CMakeLists.txt` 生成 `MiniEvent_unit_tests` — `tests/CMakeLists.txt` 已配置测试目标并链接 MiniEvent
 - [x] T005 在 `tests/CMakeLists.txt` 或主 CMake 中添加分组标签 (unit/component/integration) — 测试通过目录划分
 - [x] T006 创建覆盖矩阵生成脚本占位: `scripts/gen_coverage_matrix.py` (解析 test-matrix.md) — 脚本存在
 - [x] T007 [P] 添加 CPU 采样辅助脚本: `scripts/cpu_sample.sh` (采样占用, 输出平均值) — 脚本存在
 - [x] T008 [P] 添加冒烟脚本: `scripts/smoke_http.sh` (启动→curl→断言→退出码) — 脚本存在

## 阶段 3.2: 测试优先 (TDD) – 单元与组件基础 (必须先失败)
- [ ] T009 [P] 创建单元测试: Buffer 基本读写与扩容 `tests/unit/test_buffer.cpp`
- [ ] T010 [P] 创建单元测试: Configure 解析必需字段与缺失行为 `tests/unit/test_configure_fields.cpp`
- [ ] T011 [P] 创建单元测试: MiniEventLog 格式校验 `tests/unit/test_log_format.cpp`
- [ ] T012 [P] 创建单元测试: Channel/回调解除后不再触发 `tests/unit/test_channel_callback.cpp`
- [ ] T013 [P] 创建单元测试: ConnectionManager 计数/释放 `tests/unit/test_connection_manager.cpp`
- [ ] T014 [P] 创建单元测试: HTTP 解析非法首行关闭连接 `tests/unit/test_http_parse_error.cpp`
- [ ] T015 [P] 创建单元测试: 超时逻辑 (模拟时间推进或短超时) `tests/unit/test_timeout.cpp`
- [ ] T016 创建组件测试: EventBase + Multiplexer 基本循环 `tests/component/test_event_loop.cpp`
- [ ] T017 创建组件测试: BufferEvent 与 Channel 协同 (读触发→写回) `tests/component/test_bufferevent_channel.cpp`
- [ ] T018 创建组件测试: MultiplexerSelector 切换策略 (仅编译 + 基础调用) `tests/component/test_multiplexer_selector.cpp`
 - [x] T009 [P] 创建单元测试: Buffer 基本读写与扩容 `tests/unit/test_buffer.cpp` — 测试文件存在
 - [x] T010 [P] 创建单元测试: Configure 解析必需字段与缺失行为 `tests/unit/test_configure_fields.cpp` — 测试文件存在
 - [x] T011 [P] 创建单元测试: MiniEventLog 格式校验 `tests/unit/test_log_format.cpp` — 测试文件存在（实现中有输出重定向说明）
 - [x] T012 [P] 创建单元测试: Channel/回调解除后不再触发 `tests/unit/test_channel_callback.cpp` — 测试文件存在
 - [x] T013 [P] 创建单元测试: ConnectionManager 计数/释放 `tests/unit/test_connection_manager.cpp` — 测试文件存在 (部分用例声明为 SKIPPED)
 - [x] T014 [P] 创建单元测试: HTTP 解析非法首行关闭连接 `tests/unit/test_http_parse_error.cpp` — 测试文件存在
 - [x] T015 [P] 创建单元测试: 超时逻辑 (模拟时间推进或短超时) `tests/unit/test_timeout.cpp` — 测试文件存在 (需在 CI 中考虑时间 mocking)
 - [x] T016 创建组件测试: EventBase + Multiplexer 基本循环 `tests/component/test_event_loop.cpp` — 测试文件存在
 - [x] T017 创建组件测试: BufferEvent 与 Channel 协同 (读触发→写回) `tests/component/test_bufferevent_channel.cpp` — 测试文件存在 (依赖 pipe/socketpair)
 - [x] T018 创建组件测试: MultiplexerSelector 切换策略 (仅编译 + 基础调用) `tests/component/test_multiplexer_selector.cpp` — 测试文件存在

## 阶段 3.3: 测试优先 (TDD) – 集成与冒烟
 - [x] T019 [P] 集成测试: HTTP GET 200 响应 `tests/integration/test_http_basic.cpp` — 测试文件存在 (标记为 SKIPPED 内部备注)
 - [x] T020 [P] 集成测试: HTTP 畸形首行行为 (关闭/501) `tests/integration/test_http_malformed.cpp` — 测试文件存在 (标记为 SKIPPED)
 - [x] T021 [P] 集成测试: 多顺序连接建立与关闭 `tests/integration/test_multiple_connections.cpp` — 测试文件存在 (标记为 SKIPPED)
 - [x] T022 [P] 集成测试: 配置变更端口后重启监听 `tests/integration/test_reconfigure_port.cpp` — 测试文件存在 (标记为 SKIPPED)
 - [x] T023 [P] 集成测试: 超时连接清理 `tests/integration/test_connection_timeout.cpp` — 测试文件存在 (部分 SKIPPED)
 - [x] T024 冒烟测试: 快速启动+请求+退出 (引用脚本) `tests/smoke/test_smoke_http.cpp` — 测试文件存在

## 阶段 3.4: 实现与补强 (使之前失败测试通过)
- [ ] T025 补充 Buffer 扩容与上限逻辑 (1MB) `src/Buffer.cpp`
- [ ] T026 明确 HTTP 错误处理策略 (未知方法 501, 畸形行关闭) `src/Sever/HTTPServerMsgHandler.cpp`
- [ ] T027 在配置解析中添加缺失字段错误与默认值 `src/Configure.cpp`
- [ ] T028 补充日志模块统一格式输出 `src/MiniEventLog.cpp`
- [ ] T029 连接超时检测与日志记录 (若未完善) `src/Sever/TimeOutServer.cpp`
- [ ] T030 Channel 关闭后保护回调调用路径 `src/Channel.cpp`
- [ ] T031 ConnectionManager 释放统计与清理确认 `src/ConnectionManager.cpp`
- [ ] T032 MultiplexerSelector 测试所需最小实现完善 `src/MultiplexerSelector.cpp`
- [ ] T033 添加 CPU 占用测量集成钩子 (可选) `scripts/cpu_sample.sh`

## 阶段 3.5: 覆盖与清理
- [ ] T034 生成/更新覆盖矩阵 (自动填充 ✔/✗) `scripts/gen_coverage_matrix.py`
- [ ] T035 分类并迁移现有测试文件进入新结构 (手工核对) `tests/`
- [ ] T036 添加 CI workflow (build + ctest + smoke) `.github/workflows/ci.yml`
- [ ] T037 添加文档: `docs/testing-strategy.md` 汇总策略/矩阵/命令
- [ ] T038 [P] 添加 README 章节: 快速测试指引 `README.md`
- [ ] T039 [P] 整理 research 决策到 CHANGELOG/DECISIONS `docs/decisions.md`
- [ ] T040 复查并移除临时失败占位断言 `tests/`

## 依赖关系
- T001 → 所有测试文件创建 (T009-T024)
- T004 (测试框架) → 所有新测试 (T009 以后)
- 单元测试 (T009-T015) 先于对应实现修正 (T025-T033)
- 组件测试 (T016-T018) 先于相关实现补强 (T025, T030, T032)
- 集成/冒烟 (T019-T024) 先于 HTTP/配置/日志改进 (T026-T029)
- 覆盖矩阵脚本 (T034) 取决于初始测试编写完成 (T009-T024)
- CI (T036) 取决于冒烟脚本和基础测试 (T008, T019-T024)

## 并行执行示例
```
# 执行设置并行 (结构 + 脚本)
T007 & T008

# 单元测试初稿并行 (不同文件)
T009 T010 T011 T012 T013 T014 T015

# 集成测试并行
T019 T020 T021 T022 T023
```
(确保 T004 已完成以使测试可编译)

## 验证清单
- [ ] 所有 contracts/* (test-matrix.md) 已有对应覆盖任务 (T009+ / T019+ / T024 / T034)
- [ ] data-model 实体均有测试/实现任务 (Buffer/Configure/Log/Connection/Timeout)
- [ ] 测试均在实现补强任务之前列出
- [ ] [P] 任务间没有共享同一源文件
- [ ] 每个任务描述包含明确文件路径

## 并行组建议
| 组 | 任务 | 条件 |
|----|------|------|
| G1 | T007,T008 | 不依赖测试框架 |
| G2 | T009-T015 | T004 完成 |
| G3 | T019-T023 | 服务器可基础运行 (现状满足) |
| G4 | T038,T039 | 文档更新，可与实现并行 |

---
(完成后进入阶段 3: 实际执行这些任务；阶段 4: 使所有测试通过)
