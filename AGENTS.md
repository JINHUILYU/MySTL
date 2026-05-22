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
