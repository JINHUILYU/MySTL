# MySTL

从零实现 C++ STL 标准模板库。

## 项目结构

```
MySTL/
├── CMakeLists.txt
├── include/mystl/
│   ├── mystl.h              # 总入口头文件
│   ├── type_traits.h        # 类型萃取
│   ├── utility.h            # move / forward / swap / pair
│   ├── allocator.h          # 内存分配器
│   ├── iterator.h           # 迭代器标签与 traits
│   └── vector.h             # 动态数组容器
├── tests/
│   ├── CMakeLists.txt
│   ├── test_type_traits.cpp
│   ├── test_allocator.cpp
│   ├── test_iterator.cpp
│   └── test_vector.cpp
└── .clang-format
```

## 快速开始

### 环境要求

- CMake >= 3.14
- 支持 C++17 的编译器 (GCC >= 8, Clang >= 7, MSVC >= 2017)

### 编译与运行

```bash
# 配置项目
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 编译所有目标
cmake --build build

# 运行所有测试
ctest --test-dir build --output-on-failure

# 或者手动运行单个测试
./build/tests/test_vector
```

### 在项目中使用

MySTL 是 header-only 库，直接包含头文件即可使用：

```cpp
#include <mystl/vector.h>
#include <iostream>

int main() {
    mystl::vector<int> v = {1, 2, 3, 4, 5};

    v.push_back(6);
    v.emplace_back(7);

    for (auto& x : v) {
        std::cout << x << ' ';
    }
    // 输出: 1 2 3 4 5 6 7

    return 0;
}
```

通过 CMake 集成：

```cmake
add_subdirectory(path/to/MySTL)
target_link_libraries(your_target PRIVATE mystl)
```

命名空间为 `mystl`，所有组件与标准库 API 保持一致，方便对比学习。

### 编译单个示例

```bash
cd examples
g++ -std=c++17 -I ./include demo.cpp -o demo
```

## 已实现组件

| 头文件 | 说明 | 进度 |
|--------|------|------|
| `type_traits.h` | integral_constant, enable_if, is_same, is_integral, decay 等 40+ traits | 完成 |
| `utility.h` | move, forward, swap, pair, make_pair | 完成 |
| `allocator.h` | allocator\<T\>, allocator_traits | 完成 |
| `iterator.h` | 五种迭代器标签, iterator_traits, reverse_iterator, advance/distance/next/prev | 完成 |
| `vector.h` | 动态数组，含完整 CRUD、迭代器、移动语义优化 | 完成 |

总测试数：**75** 个，全部通过。

## 开发路线图

- [ ] string — 字符串
- [ ] algorithm — 常用算法 (sort, find, copy 等)
- [ ] list — 双向链表
- [ ] deque — 双端队列
- [ ] stack / queue / priority_queue — 容器适配器
- [ ] set / map — 红黑树容器
- [ ] unordered_set / unordered_map — 哈希表容器
- [ ] functional — 函数对象与 std::function
- [ ] smart_ptr — shared_ptr / unique_ptr / weak_ptr
- [ ] memory — allocator 扩展与智能指针辅助

## 设计原则

- **API 兼容** — 与 C++ 标准库接口一致，方便对比学习
- **渐进实现** — 按依赖关系从底层类型萃取开始，逐步往上构建
- **充分测试** — 每个组件都有对应的单元测试
- **代码清晰** — 避免过度抽象，优先可读性
