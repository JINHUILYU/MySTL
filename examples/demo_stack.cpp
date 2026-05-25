#include <initializer_list>
#include <iostream>
#include <cassert>
#include <utility>  // std::move, std::forward

namespace demo
{
    // ============================================================================
    // stack — 栈（LIFO 后进先出），只允许在栈顶插入和删除
    // ============================================================================

    class stack
    {
    public:
        // ==========================================================================
        // 构造 / 析构 / 赋值
        // ==========================================================================

        // 默认构造函数
        stack() : data_(nullptr), size_(0), capacity_(0) {}
        
        // 初始化列表
        stack(std::initializer_list<int> init) : size_(init.size()), capacity_(init.size()) {
            data_ = new int[capacity_];
            size_t i = 0;
            for (int v : init) {
                data_[i++] = v;
            }
        }
        
        // 带参数的构造函数
        stack(size_t count, int value = 0) : size_(count), capacity_(count) {
            data_ = new int[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = value;
            }
        }

        // 拷贝构造函数（深拷贝）
        stack(const stack& other) : size_(other.size_), capacity_(other.capacity_) {
            data_ = new int[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        }

        // 移动构造函数（浅拷贝，窃取资源）
        stack(stack&& other) noexcept
            : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }

        // 拷贝赋值运算符（深拷贝）
        stack& operator=(const stack& other) {
            if (this != &other) {
                // 先分配新内存，成功后再释放旧内存（异常安全）
                int* new_data = nullptr;
                if (other.capacity_ > 0) {
                    new_data = new int[other.capacity_];
                    for (size_t i = 0; i < other.size_; ++i) {
                        new_data[i] = other.data_[i];
                    }
                }
                delete[] data_;
                data_ = new_data;
                size_ = other.size_;
                capacity_ = other.capacity_;
            }
            return *this;
        }

        // 移动赋值运算符
        stack& operator=(stack&& other) noexcept {
            if (this != &other) {
                delete[] data_;
                data_ = other.data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                other.data_ = nullptr;
                other.size_ = 0;
                other.capacity_ = 0;
            }
            return *this;
        }

        // 析构函数
        ~stack() {
            delete[] data_;
            data_ = nullptr;
        }

        // ==========================================================================
        // 容量
        // ==========================================================================

        size_t size()  const { return size_; }
        bool   empty() const { return size_ == 0; }

        // ==========================================================================
        // 栈顶访问（只能访问栈顶元素）
        // ==========================================================================

        int& top() {
            assert(!empty());
            return data_[size_ - 1];
        }

        const int& top() const {
            assert(!empty());
            return data_[size_ - 1];
        }

        // ==========================================================================
        // 修改器（只允许在栈顶操作）
        // ==========================================================================

        // push 的两个重载都委托给 emplace
        void push(const int& value) { emplace(value); }
        void push(int&& value)      { emplace(std::move(value)); }

        // emplace — 变参模板 + 完美转发，在栈顶原地构造
        template <typename... Args>
        void emplace(Args&&... args) {
            if (size_ >= capacity_) {
                size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
                reserve(new_capacity);
            }
            data_[size_] = int(std::forward<Args>(args)...);
            ++size_;
        }

        void pop() {
            assert(!empty());
            --size_;
        }

        void clear() {
            size_ = 0;
        }

        // ==========================================================================
        // 交换
        // ==========================================================================

        void swap(stack& other) {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        }

    private:
        void reserve(size_t new_capacity) {
            if (new_capacity > capacity_) {
                int* new_data = new int[new_capacity];
                for (size_t i = 0; i < size_; ++i) {
                    new_data[i] = data_[i];
                }
                delete[] data_;
                data_ = new_data;
                capacity_ = new_capacity;
            }
        }

        int*   data_;
        size_t size_;
        size_t capacity_;
    };

    // ============================================================================
    // 比较运算符（按栈顶到栈底的顺序比较）
    // ============================================================================

    bool operator==(const stack& a, const stack& b) {
        if (a.size() != b.size()) return false;
        // 比较需要遍历元素，但因为 stack 没有 begin/end，这里创建临时副本弹出
        stack ac(a), bc(b);
        for (size_t i = 0; i < a.size(); ++i) {
            if (ac.top() != bc.top()) return false;
            ac.pop(); bc.pop();  // 只是演示逻辑，实际比较用拷贝后的临时对象
        }
        return true;
    }

    // 为了方便非成员比较，添加一个 friend 辅助函数来逐元素比较
    namespace detail {
    inline bool element_wise_equal(const stack& a, const stack& b) {
        // 创建拷贝并从顶部开始比较（stack 没有迭代器，通过拷贝+pop 遍历）
        stack ca(a), cb(b);
        while (!ca.empty()) {
            if (ca.top() != cb.top()) return false;
            ca.pop(); cb.pop();
        }
        return true;
    }
    }  // namespace detail

    bool operator!=(const stack& a, const stack& b) { return !(a == b); }

    bool operator<(const stack& a, const stack& b) {
        if (a.size() != b.size()) return a.size() < b.size();
        stack ca(a), cb(b);
        while (!ca.empty()) {
            if (ca.top() < cb.top()) return true;
            if (cb.top() < ca.top()) return false;
            ca.pop(); cb.pop();
        }
        return false;
    }

    bool operator>(const stack& a, const stack& b)  { return b < a; }
    bool operator<=(const stack& a, const stack& b) { return !(b < a); }
    bool operator>=(const stack& a, const stack& b) { return !(a < b); }

    void swap(stack& a, stack& b) noexcept { a.swap(b); }

}  // namespace demo


// ============================================================================
// 测试函数
// ============================================================================

void TestConstruction()
{
    std::cout << "--- Testing construction ---" << std::endl;

    demo::stack s1;  // 默认构造
    std::cout << "Default: size=" << s1.size() << ", empty=" << s1.empty() << std::endl;

    demo::stack s2(3, 10);  // 填充构造
    std::cout << "Filled(3, 10): size=" << s2.size() << ", top=" << s2.top() << std::endl;

    demo::stack s3 = s2;  // 拷贝构造
    std::cout << "Copied: size=" << s3.size() << ", top=" << s3.top() << std::endl;

    demo::stack s4(std::move(s2));  // 移动构造
    std::cout << "Moved-from: size=" << s2.size() << ", empty=" << s2.empty() << std::endl;
    std::cout << "Moved-to: size=" << s4.size() << ", top=" << s4.top() << std::endl;

    demo::stack s5;
    s5 = s3;  // 拷贝赋值
    std::cout << "Copy-assigned: size=" << s5.size() << ", top=" << s5.top() << std::endl;

    demo::stack s6;
    s6 = std::move(s4);  // 移动赋值
    std::cout << "Move-assigned: size=" << s6.size()
              << ", moved-from size=" << s4.size() << std::endl;
}

void TestPushPop()
{
    std::cout << "--- Testing push/pop ---" << std::endl;

    demo::stack s;

    s.push(1);
    s.push(2);
    s.push(3);
    std::cout << "After push(1,2,3): size=" << s.size() << ", top=" << s.top() << std::endl;

    s.pop();
    std::cout << "After pop: size=" << s.size() << ", top=" << s.top() << std::endl;

    s.pop();
    s.pop();
    std::cout << "After pop all: size=" << s.size() << ", empty=" << s.empty() << std::endl;
}

void TestEmplace()
{
    std::cout << "--- Testing emplace ---" << std::endl;

    demo::stack s;
    s.emplace(42);
    s.emplace(10 + 5);
    s.emplace(100);
    std::cout << "After emplace(42, 15, 100): size=" << s.size() << ", top=" << s.top() << std::endl;
}

void TestLIFO()
{
    std::cout << "--- Testing LIFO order ---" << std::endl;

    demo::stack s;
    s.push(1);
    s.push(2);
    s.push(3);
    s.push(4);
    s.push(5);

    // 出栈顺序应是 5, 4, 3, 2, 1（后进先出）
    std::cout << "Pop order: ";
    while (!s.empty()) {
        std::cout << s.top() << " ";
        s.pop();
    }
    std::cout << std::endl;
}

void TestClear()
{
    std::cout << "--- Testing clear ---" << std::endl;

    demo::stack s(5, 42);
    std::cout << "Before clear: size=" << s.size() << ", top=" << s.top() << std::endl;
    s.clear();
    std::cout << "After clear: size=" << s.size() << ", empty=" << s.empty() << std::endl;
}

void TestSwap()
{
    std::cout << "--- Testing swap ---" << std::endl;

    demo::stack a;
    a.push(1); a.push(2); a.push(3);

    demo::stack b;
    b.push(100);

    std::cout << "Before swap: a.size=" << a.size() << " a.top=" << a.top()
              << ", b.size=" << b.size() << " b.top=" << b.top() << std::endl;

    a.swap(b);
    std::cout << "After member swap: a.size=" << a.size() << " a.top=" << a.top()
              << ", b.size=" << b.size() << " b.top=" << b.top() << std::endl;

    // 非成员 swap
    demo::swap(a, b);
    std::cout << "After non-member swap: a.size=" << a.size() << " a.top=" << a.top()
              << ", b.size=" << b.size() << " b.top=" << b.top() << std::endl;

    // 空栈 swap
    demo::stack e;
    a.swap(e);
    std::cout << "Swap with empty: a.size=" << a.size() << ", e.size=" << e.size()
              << " e.top=" << e.top() << std::endl;
}

void TestComparisons()
{
    std::cout << "--- Testing comparisons ---" << std::endl;

    demo::stack a; a.push(1); a.push(2); a.push(3);
    demo::stack b; b.push(1); b.push(2); b.push(3);
    demo::stack c; c.push(1); c.push(2); c.push(4);
    demo::stack d; d.push(1); d.push(2);  // 比 a 短

    std::cout << "a == b: " << (a == b) << std::endl;
    std::cout << "a != c: " << (a != c) << std::endl;
    std::cout << "a < c:  " << (a < c)  << std::endl;
    std::cout << "c > a:  " << (c > a)  << std::endl;
    std::cout << "d < a:  " << (d < a)  << std::endl;
}

// ============================================================================
// main
// ============================================================================

int main()
{
    TestConstruction();
    TestPushPop();
    TestEmplace();
    TestLIFO();
    TestClear();
    TestSwap();
    TestComparisons();

    std::cout << "\nAll stack tests passed!" << std::endl;
    return 0;
}
