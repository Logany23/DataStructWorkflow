# DataStructWorkflow (C)

DataStructWorkflow 是一个基于 C 语言实现的命令行任务调度器，用于演示“**依赖图约束 + 优先级策略**”下的执行编排。  
它把多个经典数据结构组合在同一条业务链路中，适合作为数据结构课程项目或简历项目。

## 项目亮点

- 多数据结构协同：哈希表 + 图 + 队列 + 最小堆 + 栈
- 支持依赖建模与拓扑调度，自动检测环依赖
- 在可执行任务集合中按优先级动态选择下一任务
- 代码模块清晰（`main.c` / `scheduler.c` / `scheduler.h`），便于扩展

## 数据结构设计

- 哈希表：任务 ID 映射到内部索引，降低查找与校验成本
- 图（邻接表）：描述任务依赖边 `from -> to`
- 入度数组：配合拓扑排序识别当前可执行节点
- 队列：收集初始入度为 0 的任务
- 最小堆：在就绪任务中按优先级弹出（优先级越小越先执行）
- 栈：记录执行轨迹并输出逆序完成序列

## 时间复杂度（核心流程）

- 新增任务：平均 `O(1)`（哈希定位）
- 新增依赖：`O(1)`
- 调度执行：`O((V + E) + V log V)`  
  其中 `V` 为任务数，`E` 为依赖边数，`log V` 来自最小堆操作

## 构建与运行

### Windows (gcc / mingw)

```bash
gcc -std=c11 -Wall -Wextra -O2 main.c scheduler.c -o workflow.exe
workflow.exe
```

### Linux / macOS

```bash
gcc -std=c11 -Wall -Wextra -O2 main.c scheduler.c -o workflow
./workflow
```

## 命令说明

```text
add_task <id> <priority> <duration>   # 新增任务
add_dep  <from_id> <to_id>            # 新增依赖: from 必须先于 to
print                                  # 打印任务与入度信息
run                                    # 执行调度
demo                                   # 加载内置示例
help                                   # 查看帮助
exit                                   # 退出程序
```

## 快速体验

运行后依次输入：

```text
demo
print
run
```

你会看到带时间戳的调度输出，以及逆序完成轨迹（栈输出）。

## 可扩展方向

- 支持任务状态持久化（文件/数据库）
- 增加多线程 Worker 模拟并行执行
- 加入任务失败重试与超时策略
- 支持 DOT/Graphviz 导出依赖图
