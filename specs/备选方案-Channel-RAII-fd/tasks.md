# 任务: 备选方案-Channel 通过RAII管理fd

**输入**: 来自 `/specs/备选方案-Channel-RAII-fd/` 的设计文档
**先决条件**: data-model.md, research.md

## 执行流程 (主流程)
```
1. 阅读 research.md，理解当前fd生命周期管理的设计动机。
2. 阅读 data-model.md，明确Channel的fd成员及其用途。
3. 修改Channel类：
   → 构造函数接收fd
   → 析构函数中close(fd)
4. 修改所有Channel的创建和销毁逻辑，确保fd只被关闭一次。
5. 编写/修改单元测试，验证fd生命周期管理的正确性。
6. 对比分析：写一段对比说明，解释RAII与当前设计的优缺点。
7. 完善文档。
```

## 任务列表

- [T001] [P] 阅读 research.md，理解fd生命周期管理的设计动机  
  路径: specs/002-generate-detailed-code/research.md

- [T002] [P] 阅读 data-model.md，明确Channel的fd成员及其用途  
  路径: specs/002-generate-detailed-code/data-model.md

- [T003] 修改Channel类，实现RAII管理fd  
  路径: include/Channel.hpp, src/Channel.cpp
  依赖: T001, T002

- [T004] 修改所有Channel创建和销毁逻辑，确保fd只被关闭一次  
  路径: src/ 及相关测试代码
  依赖: T003

- [T005] [P] 编写/修改单元测试，验证fd生命周期管理正确性  
  路径: tests/unit/
  依赖: T004

- [T006] [P] 对比分析：写一段对比说明，解释RAII与当前设计的优缺点  
  路径: specs/备选方案-Channel-RAII-fd/compare.md
  依赖: T003

- [T007] 完善文档  
  路径: specs/备选方案-Channel-RAII-fd/
  依赖: T006

## 并行执行示例

- T001, T002, T005, T006 可并行执行
- T003 → T004 → T007 顺序执行

## Task agent 命令示例

```bash
# 并行
agent run T001 & agent run T002 & agent run T005 & agent run T006
# 顺序
agent run T003 && agent run T004 && agent run T007
```
