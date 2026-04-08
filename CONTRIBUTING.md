# Contributing

感谢你对 DataStructWorkflow 的关注。

## 开发流程

- Fork 本仓库并创建特性分支：`feat/<your-topic>`
- 提交前请确保可以编译运行
- 发起 PR，并清晰描述改动动机与测试方式

## 本地构建

```bash
gcc -std=c11 -Wall -Wextra -O2 main.c scheduler.c -o workflow.exe
```

## 代码约定

- 保持 C11 标准兼容
- 新增逻辑优先放在 `scheduler.c`，`main.c` 保持交互入口职责
- 命名清晰，避免单字符变量（循环变量除外）
- 对复杂逻辑补充必要注释（简短、解释原因）

## Commit 建议

- `feat:` 新功能
- `fix:` 缺陷修复
- `refactor:` 重构
- `docs:` 文档更新
- `test:` 测试相关
