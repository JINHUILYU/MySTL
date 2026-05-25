#include <initializer_list>
#include <iostream>
#include <cassert>
#include <utility>  // std::move, std::swap

namespace demo
{

    // ============================================================================
    // array<T, N> — 固定大小数组（类似 std::array），数据存储在对象内部，无动态分配
    // ============================================================================
    template <typename T, size_t N>
    class array
    {
    public:
        // 数据存储在对象内部（栈上），N==0 时仍需至少 1 个元素的空间
        T data_[N > 0 ? N : 1];

        // ==========================================================================
        // 元素访问
        // ==========================================================================

        T& operator[](size_t index) {
            assert(index < N);
            return data_[index];
        }

        const T& operator[](size_t index) const {
            assert(index < N);
            return data_[index];
        }

        T& at(size_t index) {
            if (index >= N)
                throw "array::at: index out of range";
            return data_[index];
        }

        const T& at(size_t index) const {
            if (index >= N)
                throw "array::at: index out of range";
            return data_[index];
        }

        T& front() {
            assert(N > 0);
            return data_[0];
        }

        const T& front() const {
            assert(N > 0);
            return data_[0];
        }

        T& back() {
            assert(N > 0);
            return data_[N - 1];
        }

        const T& back() const {
            assert(N > 0);
            return data_[N - 1];
        }

        T* data() { return data_; }
        const T* data() const { return data_; }

        // ==========================================================================
        // 迭代器
        // ==========================================================================

        typedef T* iterator;
        iterator begin() { return data_; }
        iterator end()   { return data_ + N; }

        typedef const T* const_iterator;
        const_iterator begin() const { return data_; }
        const_iterator end()   const { return data_ + N; }

        // ==========================================================================
        // 容量
        // ==========================================================================

        size_t size()     const { return N; }
        size_t max_size() const { return N; }
        bool   empty()    const { return N == 0; }

        // ==========================================================================
        // 修改器
        // ==========================================================================

        void fill(const T& value) {
            for (size_t i = 0; i < N; ++i)
                data_[i] = value;
        }

        void swap(array& other) {
            for (size_t i = 0; i < N; ++i)
                std::swap(data_[i], other.data_[i]);
        }
    };

    // ============================================================================
    // 比较运算符
    // ============================================================================

    template <typename T, size_t N>
    bool operator==(const array<T, N>& a, const array<T, N>& b) {
        for (size_t i = 0; i < N; ++i)
            if (a[i] != b[i]) return false;
        return true;
    }

    template <typename T, size_t N>
    bool operator!=(const array<T, N>& a, const array<T, N>& b) { return !(a == b); }

    template <typename T, size_t N>
    bool operator<(const array<T, N>& a, const array<T, N>& b) {
        for (size_t i = 0; i < N; ++i) {
            if (a[i] < b[i]) return true;
            if (b[i] < a[i]) return false;
        }
        return false;
    }

    template <typename T, size_t N>
    bool operator>(const array<T, N>& a, const array<T, N>& b)  { return b < a; }

    template <typename T, size_t N>
    bool operator<=(const array<T, N>& a, const array<T, N>& b) { return !(b < a); }

    template <typename T, size_t N>
    bool operator>=(const array<T, N>& a, const array<T, N>& b) { return !(a < b); }

    template <typename T, size_t N>
    void swap(array<T, N>& a, array<T, N>& b) { a.swap(b); }

}  // namespace demo


// ============================================================================
// 测试函数
// ============================================================================

void TestAggregateInit()
{
    std::cout << "--- Testing aggregate initialization ---" << std::endl;

    // 完整初始化
    demo::array<int, 5> arr = {10, 20, 30, 40, 50};
    std::cout << "Full init: ";
    for (size_t i = 0; i < arr.size(); ++i) std::cout << arr[i] << " ";
    std::cout << std::endl;

    // 部分初始化（剩余元素值初始化为 0）
    demo::array<int, 5> partial = {1, 2};
    std::cout << "Partial init: ";
    for (size_t i = 0; i < partial.size(); ++i) std::cout << partial[i] << " ";
    std::cout << std::endl;

    // 不同类型
    demo::array<double, 3> darr = {1.1, 2.2, 3.3};
    std::cout << "Double: ";
    for (size_t i = 0; i < darr.size(); ++i) std::cout << darr[i] << " ";
    std::cout << std::endl;

    // 零长度 array
    demo::array<int, 0> empty_arr;
    std::cout << "N=0 array: size=" << empty_arr.size()
              << ", empty=" << empty_arr.empty() << std::endl;
}

void TestElementAccess()
{
    std::cout << "--- Testing element access ---" << std::endl;

    demo::array<int, 5> arr = {10, 20, 30, 40, 50};

    std::cout << "front: " << arr.front() << std::endl;
    std::cout << "back:  " << arr.back()  << std::endl;
    std::cout << "[2]:   " << arr[2]      << std::endl;
    std::cout << "at(3): " << arr.at(3)   << std::endl;

    // 越界抛异常
    try {
        arr.at(100);
    } catch (const char* e) {
        std::cout << "at(100) threw: " << e << std::endl;
    }

    // data() 返回底层指针
    std::cout << "data()[0]: " << *arr.data() << std::endl;

    // 通过引用修改元素
    arr.front() = 100;
    arr.back()  = 500;
    std::cout << "After front=100 back=500: front=" << arr.front()
              << ", back=" << arr.back() << std::endl;
}

void TestIterators()
{
    std::cout << "--- Testing iterators ---" << std::endl;

    demo::array<int, 5> arr = {1, 2, 3, 4, 5};

    // 前向遍历
    std::cout << "Forward:  ";
    for (auto it = arr.begin(); it != arr.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    // range-for
    std::cout << "Range-for: ";
    for (const auto& v : arr) std::cout << v << " ";
    std::cout << std::endl;

    // const 迭代器
    std::cout << "Const:    ";
    for (auto ci = arr.begin(); ci != arr.end(); ++ci) std::cout << *ci << " ";
    std::cout << std::endl;

    // 指针即迭代器，支持随机访问
    auto it = arr.begin();
    std::cout << "*(begin+3): " << *(it + 3) << std::endl;
    std::cout << "begin[2]:   " << it[2]      << std::endl;

    // 通过迭代器写入
    *arr.begin() = 99;
    std::cout << "After write via iterator: front=" << arr.front() << std::endl;
}

void TestCapacity()
{
    std::cout << "--- Testing capacity ---" << std::endl;

    demo::array<int, 4> arr = {1, 2, 3, 4};
    std::cout << "size:     " << arr.size()     << std::endl;
    std::cout << "max_size: " << arr.max_size() << std::endl;
    std::cout << "empty:    " << arr.empty()    << std::endl;

    // 零长度 array
    demo::array<int, 0> empty_arr;
    std::cout << "N=0 empty: " << empty_arr.empty() << std::endl;
}

void TestFill()
{
    std::cout << "--- Testing fill ---" << std::endl;

    demo::array<int, 4> arr = {1, 2, 3, 4};
    std::cout << "Before fill: ";
    for (auto v : arr) std::cout << v << " ";
    std::cout << std::endl;

    arr.fill(42);
    std::cout << "After fill(42): ";
    for (auto v : arr) std::cout << v << " ";
    std::cout << std::endl;
}

void TestSwap()
{
    std::cout << "--- Testing swap ---" << std::endl;

    demo::array<int, 3> a = {1, 2, 3};
    demo::array<int, 3> b = {10, 20, 30};

    std::cout << "Before swap: a=[" << a[0] << "," << a[1] << "," << a[2]
              << "] b=[" << b[0] << "," << b[1] << "," << b[2] << "]" << std::endl;

    a.swap(b);
    std::cout << "After member swap: a=[" << a[0] << "," << a[1] << "," << a[2]
              << "] b=[" << b[0] << "," << b[1] << "," << b[2] << "]" << std::endl;

    // 非成员 swap
    demo::swap(a, b);
    std::cout << "After non-member swap: a=[" << a[0] << "," << a[1] << "," << a[2]
              << "] b=[" << b[0] << "," << b[1] << "," << b[2] << "]" << std::endl;
}

void TestComparisons()
{
    std::cout << "--- Testing comparisons ---" << std::endl;

    demo::array<int, 3> a = {1, 2, 3};
    demo::array<int, 3> b = {1, 2, 3};
    demo::array<int, 3> c = {1, 2, 4};

    std::cout << "a == b: " << (a == b) << std::endl;
    std::cout << "a != c: " << (a != c) << std::endl;
    std::cout << "a < c:  " << (a < c)  << std::endl;
    std::cout << "c > a:  " << (c > a)  << std::endl;
    std::cout << "a <= b: " << (a <= b) << std::endl;
}

// ============================================================================
// main
// ============================================================================

int main()
{
    TestAggregateInit();
    TestElementAccess();
    TestIterators();
    TestCapacity();
    TestFill();
    TestSwap();
    TestComparisons();

    std::cout << "\nAll array tests passed!" << std::endl;
    return 0;
}
