# MySTL — Agent Instructions

从零实现 C++ STL 的 header-only 教学库。所有组件与标准库 API 保持一致，命名空间为 `mystl`。

## 构建 & 测试

```bash
# 配置（首次或 CMake 变更后）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 编译
cmake --build build

# 运行全部测试
ctest --test-dir build --output-on-failure

# 运行单个测试可执行文件
./build/tests/test_vector
```

测试框架：**Google Test**（`FetchContent` 自动拉取，无需手动安装）。  
新增测试时向 [tests/CMakeLists.txt](tests/CMakeLists.txt) 追加 `add_executable` + `target_link_libraries` + `add_test`。

## 代码规范

- **C++ 标准**：C++17，禁止使用编译器扩展（`CMAKE_CXX_EXTENSIONS OFF`）
- **命名空间**：所有公共 API 位于 `mystl::`
- **API 镜像标准库**：新容器/算法的接口必须与 `std::` 对应类保持一致
- **私有成员**：尾部下划线，如 `begin_`、`end_`、`cap_`
- **类型别名**：用 `using` 而非 `typedef`；模板别名用 `_t` 后缀（`enable_if_t`、`remove_reference_t`）

## 架构

| 文件 | 职责 |
|------|------|
| [include/mystl/type_traits.h](include/mystl/type_traits.h) | `integral_constant`、`enable_if`、40+ 类型谓词，SFINAE 基础 |
| [include/mystl/utility.h](include/mystl/utility.h) | `move`、`forward`、`swap`、`pair`、`make_pair` |
| [include/mystl/allocator.h](include/mystl/allocator.h) | `allocator<T>`、`allocator_traits<Alloc>` |
| [include/mystl/iterator.h](include/mystl/iterator.h) | 迭代器标签、`iterator_traits`、`reverse_iterator` |
| [include/mystl/vector.h](include/mystl/vector.h) | `vector<T, Alloc>`，三指针设计（`begin_/end_/cap_`） |
| [include/mystl/mystl.h](include/mystl/mystl.h) | 总入口，包含所有头文件 |

**依赖顺序**：`type_traits` → `utility` → `allocator` / `iterator` → 容器

## 模板编程模式

- **SFINAE**：用自定义 `enable_if_t`（而非 `std::`）控制重载
- **Traits 接口**：通过 `allocator_traits`、`iterator_traits` 访问分配器和迭代器，而非直接调用成员函数
- **移动语义**：grow 时用 `is_nothrow_move_constructible` 判断能否 move（否则 copy）
- **完美转发**：用自定义 `mystl::forward` / `mystl::move`

## 添加新组件的步骤

1. 在 `include/mystl/` 创建 `<component>.h`，使用 `#pragma once`
2. 在 `include/mystl/mystl.h` 添加 `#include`
3. 在 `tests/` 创建 `test_<component>.cpp`，用 `Tracked`/`Counter` 等辅助结构验证拷贝/移动语义
4. 在 [tests/CMakeLists.txt](tests/CMakeLists.txt) 注册测试目标
5. 运行 `cmake --build build && ctest --test-dir build --output-on-failure` 验证

## Response Language

- Default to Chinese in responses.
- If the user explicitly asks for another language, follow the user's request.

## General Assistant Behavior

From now on, act as my expert assistant with access to all your reasoning and knowledge. Always provide:

- A clear, direct answer to my request.
- A step-by-step explanation of how you got there.
- Alternative perspectives or solutions I might not have thought of.
- A practical summary or action plan I can apply immediately.

Never give vague answers. If the question is broad, break it into parts. If I ask for help, act like a professional in that domain (teacher, coach, engineer, doctor, etc.). Push your reasoning to 100% of your capacity.

## Git 提交规范

- 提交信息格式：`<type>: <简短描述>`，描述默认使用中文。若用户明确要求使用其他语言（如英文），则以用户要求为准。
- type 取值：
  - feat: 新功能
  - fix: 修复 bug
  - docs: 仅文档变更
  - style: 代码风格变动（不影响代码逻辑，如格式化、缩进等）
  - refactor: 代码重构（既不是新增功能也不是修复 bug）
  - perf: 性能优化
  - test: 添加或修改测试
  - chore: 杂项（构建过程、依赖、辅助工具等）
  - build: 构建系统或外部依赖项变更
  - ci: 持续集成配置变更
  - revert: 回滚之前的提交
- 每次 commit 仅包含与该提交主题直接相关的文件更改，避免一次 commit 包含过多内容，便于后续问题排查。

# andrej-karpathy-style-guidelines

Behavioral guidelines to reduce common LLM coding mistakes. Merge with project-specific instructions as needed.

**Tradeoff:** These guidelines bias toward caution over speed. For trivial tasks, use judgment.

## 1. Think Before Coding

**Don't assume. Don't hide confusion. Surface tradeoffs.**

Before implementing:
- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

## 2. Simplicity First

**Minimum code that solves the problem. Nothing speculative.**

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.

Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

## 3. Surgical Changes

**Touch only what you must. Clean up only your own mess.**

When editing existing code:
- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:
- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

The test: Every changed line should trace directly to the user's request.

## 4. Goal-Driven Execution

**Define success criteria. Loop until verified.**

Transform tasks into verifiable goals:
- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:
```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]
```

Strong success criteria let you loop independently. Weak criteria ("make it work") require constant clarification.

---

**These guidelines are working if:** fewer unnecessary changes in diffs, fewer rewrites due to overcomplication, and clarifying questions come before implementation rather than after mistakes.