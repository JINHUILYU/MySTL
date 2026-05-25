#include <initializer_list>
#include <iostream>
#include <cassert>
#include <new>      // placement new
#include <utility>  // std::move, std::forward

namespace demo
{
    class vector
    {
    public:
        // 默认构造函数
        vector() : data_(nullptr), size_(0), capacity_(0) {}

        // 初始化列表
        vector(std::initializer_list<int> init) : size_(init.size()), capacity_(init.size()) {
            data_ = new int[capacity_];
            size_t i = 0;
            for (int v : init) {
                data_[i++] = v;
            }
        }

        // 带参数的构造函数
        vector(size_t count, int value = 0) : size_(count), capacity_(count) {
            data_ = new int[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = value;
            }
        }

        // 拷贝构造函数（深拷贝）
        vector(const vector& other) : size_(other.size_), capacity_(other.capacity_) {
            data_ = new int[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        }

        // 移动构造函数（浅拷贝，窃取资源）
        vector(vector&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }

        // 拷贝赋值运算符（深拷贝）
        vector& operator=(const vector& other) {
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
        vector& operator=(vector&& other) noexcept {
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
        ~vector() {
            clear();
            delete[] data_;
            data_ = nullptr;
        }

        // 非 const 迭代器
        typedef int* iterator;
        iterator begin() { return data_; }
        iterator end() { return data_ + size_; }

        // const 迭代器
        typedef const int* const_iterator;
        const_iterator begin() const { return data_; }
        const_iterator end() const { return data_ + size_; }

        // ==========================================================================
        // 元素访问
        // ==========================================================================

        int& operator[](size_t index) {
            assert(index < size_);
            return data_[index];
        }

        const int& operator[](size_t index) const {
            assert(index < size_);
            return data_[index];
        }

        int& at(size_t index) {
            if (index >= size_) throw "vector::at: index out of range";
            return data_[index];
        }

        const int& at(size_t index) const {
            if (index >= size_) throw "vector::at: index out of range";
            return data_[index];
        }

        int& front() {
            assert(size_ > 0);
            return data_[0];
        }

        const int& front() const {
            assert(size_ > 0);
            return data_[0];
        }

        int& back() {
            assert(size_ > 0);
            return data_[size_ - 1];
        }

        const int& back() const {
            assert(size_ > 0);
            return data_[size_ - 1];
        }

        int* data() { return data_; }
        const int* data() const { return data_; }

        // ==========================================================================
        // 容量管理
        // ==========================================================================

        size_t size() const { return size_; }
        size_t capacity() const { return capacity_; }
        bool empty() const { return size_ == 0; }

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

        void shrink_to_fit() {
            if (capacity_ > size_) {
                if (size_ == 0) {
                    delete[] data_;
                    data_ = nullptr;
                    capacity_ = 0;
                } else {
                    int* new_data = new int[size_];
                    for (size_t i = 0; i < size_; ++i) {
                        new_data[i] = data_[i];
                    }
                    delete[] data_;
                    data_ = new_data;
                    capacity_ = size_;
                }
            }
        }

        // ==========================================================================
        // 修改器
        // ==========================================================================

        // push_back 的两个重载都委托给 emplace_back
        void push_back(const int& value) { emplace_back(value); }
        void push_back(int&& value)      { emplace_back(std::move(value)); }

        // emplace_back — 变参模板 + 完美转发，在 vector 内部原地构造
        template <typename... Args>
        void emplace_back(Args&&... args) {
            if (size_ >= capacity_) {
                size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
                reserve(new_capacity);
            }
            // placement new：直接在 data_[size_] 位置构造对象，零临时量
            new (data_ + size_) int(std::forward<Args>(args)...);
            ++size_;
        }

        void pop_back() {
            assert(size_ > 0);
            --size_;
        }

        void insert(size_t pos, int value) {
            assert(pos <= size_);
            if (size_ >= capacity_) {
                size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
                int* new_data = new int[new_capacity];
                for (size_t i = 0; i < pos; ++i) {
                    new_data[i] = data_[i];
                }
                new_data[pos] = value;
                for (size_t i = pos; i < size_; ++i) {
                    new_data[i + 1] = data_[i];
                }
                delete[] data_;
                data_ = new_data;
                capacity_ = new_capacity;
            } else {
                for (size_t i = size_; i > pos; --i) {
                    data_[i] = data_[i - 1];
                }
                data_[pos] = value;
            }
            ++size_;
        }

        void erase(size_t pos) {
            assert(pos < size_);
            for (size_t i = pos; i < size_ - 1; ++i) {
                data_[i] = data_[i + 1];
            }
            --size_;
        }

        void erase(size_t first, size_t last) {
            assert(first <= last && last <= size_);
            if (first == last) return;
            size_t count = last - first;
            for (size_t i = first; i < size_ - count; ++i) {
                data_[i] = data_[i + count];
            }
            size_ -= count;
        }

        void resize(size_t new_size, int value = 0) {
            if (new_size < size_) {
                size_ = new_size;
            } else if (new_size > size_) {
                if (new_size > capacity_) {
                    reserve(new_size);
                }
                for (size_t i = size_; i < new_size; ++i) {
                    data_[i] = value;
                }
                size_ = new_size;
            }
        }

        void clear() {
            size_ = 0;
        }

        // ==========================================================================
        // 交换
        // ==========================================================================

        void swap(vector& other) {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        }

    private:
        int* data_;
        size_t size_;
        size_t capacity_;
    };

    // ============================================================================
    // 比较运算符
    // ============================================================================

    bool operator==(const vector& a, const vector& b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) return false;
        }
        return true;
    }

    bool operator!=(const vector& a, const vector& b) { return !(a == b); }

    bool operator<(const vector& a, const vector& b) {
        size_t n = a.size() < b.size() ? a.size() : b.size();
        for (size_t i = 0; i < n; ++i) {
            if (a[i] < b[i]) return true;
            if (b[i] < a[i]) return false;
        }
        return a.size() < b.size();
    }

    bool operator>(const vector& a, const vector& b)  { return b < a; }
    bool operator<=(const vector& a, const vector& b) { return !(b < a); }
    bool operator>=(const vector& a, const vector& b) { return !(a < b); }

}  // namespace demo

// ============================================================================
// 测试函数
// ============================================================================

void TestConstruction()
{
    std::cout << "--- Testing construction ---" << std::endl;

    demo::vector v1;  // 默认构造
    std::cout << "Default: size=" << v1.size() << ", empty=" << v1.empty() << std::endl;

    demo::vector v2(3, 10);  // 填充构造
    std::cout << "Filled: ";
    for (auto it = v2.begin(); it != v2.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    demo::vector v3 = v2;  // 拷贝构造
    std::cout << "Copied: size=" << v3.size() << std::endl;

    demo::vector v4(std::move(v2));  // 移动构造
    std::cout << "Moved-from: size=" << v2.size() << ", empty=" << v2.empty() << std::endl;
    std::cout << "Moved-to: size=" << v4.size() << std::endl;
}

void TestElementAccess()
{
    std::cout << "--- Testing element access ---" << std::endl;

    demo::vector v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    std::cout << "front: " << v.front() << std::endl;
    std::cout << "back: " << v.back() << std::endl;
    std::cout << "at(1): " << v.at(1) << std::endl;
    std::cout << "data()[0]: " << *v.data() << std::endl;

    try {
        v.at(100);
    } catch (const char* e) {
        std::cout << "at(100) threw: " << e << std::endl;
    }
}

void TestCapacity()
{
    std::cout << "--- Testing capacity ---" << std::endl;

    demo::vector v;
    v.reserve(100);
    std::cout << "After reserve(100): capacity=" << v.capacity() << ", size=" << v.size() << std::endl;

    v.push_back(1);
    v.push_back(2);
    v.shrink_to_fit();
    std::cout << "After shrink_to_fit: capacity=" << v.capacity() << ", size=" << v.size() << std::endl;
}

void TestModifiers()
{
    std::cout << "--- Testing modifiers ---" << std::endl;

    demo::vector v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    std::cout << "After push_back: ";
    for (size_t i = 0; i < v.size(); ++i) std::cout << v[i] << " ";
    std::cout << std::endl;

    v.pop_back();
    std::cout << "After pop_back: size=" << v.size() << ", back=" << v.back() << std::endl;

    v.insert(1, 99);
    std::cout << "After insert(1, 99): ";
    for (size_t i = 0; i < v.size(); ++i) std::cout << v[i] << " ";
    std::cout << std::endl;

    v.erase(0);
    std::cout << "After erase(0): ";
    for (size_t i = 0; i < v.size(); ++i) std::cout << v[i] << " ";
    std::cout << std::endl;

    demo::vector v2;
    v2.push_back(10);
    v2.push_back(20);
    v2.push_back(30);
    v2.push_back(40);
    v2.erase(1, 3);  // 删除 [1, 3)，即删除元素 20 和 30
    std::cout << "After erase(1, 3): ";
    for (size_t i = 0; i < v2.size(); ++i) std::cout << v2[i] << " ";
    std::cout << std::endl;

    v2.resize(5, 9);
    std::cout << "After resize(5, 9): ";
    for (size_t i = 0; i < v2.size(); ++i) std::cout << v2[i] << " ";
    std::cout << std::endl;

    v2.resize(2);
    std::cout << "After resize(2): ";
    for (size_t i = 0; i < v2.size(); ++i) std::cout << v2[i] << " ";
    std::cout << std::endl;

    v2.clear();
    std::cout << "After clear: size=" << v2.size() << ", empty=" << v2.empty() << std::endl;
}

void TestComparisons()
{
    std::cout << "--- Testing comparisons ---" << std::endl;

    demo::vector a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);

    demo::vector b;
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);

    demo::vector c;
    c.push_back(1);
    c.push_back(2);
    c.push_back(4);

    std::cout << "a == b: " << (a == b) << std::endl;
    std::cout << "a != c: " << (a != c) << std::endl;
    std::cout << "a < c:  " << (a < c) << std::endl;
    std::cout << "c > a:  " << (c > a) << std::endl;
}

void TestSwap()
{
    std::cout << "--- Testing swap ---" << std::endl;

    demo::vector a;
    a.push_back(1);
    a.push_back(2);

    demo::vector b;
    b.push_back(100);

    std::cout << "Before swap: a.size=" << a.size() << ", b.size=" << b.size() << std::endl;
    a.swap(b);
    std::cout << "After swap: a.size=" << a.size() << ", b.size=" << b.size() << std::endl;
    std::cout << "a[0]=" << a[0] << ", b[0]=" << b[0] << std::endl;
}

int main() {
    TestConstruction();
    TestElementAccess();
    TestCapacity();
    TestModifiers();
    TestComparisons();
    TestSwap();
    return 0;
}
