# DataStructWorkflow (C)

一个偏中高级的 C 语言命令行项目：**任务依赖调度器**。  
它把多个经典数据结构组合在一个真实场景里（工作流编排）。

## 用到的数据结构

- **哈希表**：任务 ID -> 任务索引，快速查找与去重
- **图（邻接表）**：表示任务依赖关系（A 必须先于 B）
- **队列**：初始化拓扑起点（入度为 0 的任务）
- **最小堆**：在“可执行任务集合”中按优先级选择下一任务
- **栈**：输出逆序完成轨迹

## 功能

- 新增任务（优先级 + 预计耗时）
- 新增依赖边
- 打印当前任务状态
- 执行拓扑 + 优先级联合调度
- 检测依赖环

## 构建与运行（Windows + gcc）

```bash
gcc -std=c11 -Wall -Wextra -O2 main.c scheduler.c -o workflow.exe
workflow.exe
```

## 交互命令

```text
add_task <id> <priority> <duration>
add_dep  <from_id> <to_id>
print
run
demo
help
exit
```

## 快速体验

启动后输入：

```text
demo
print
run
```

会看到按“依赖约束 + 优先级”的执行顺序。
