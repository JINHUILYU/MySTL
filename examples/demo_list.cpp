#include <initializer_list>
#include <iostream>
#include <cassert>
#include <utility>  // std::move, std::swap

namespace demo
{
    class list
    {
    public:
        // ==========================================================================
        // 节点结构
        // ==========================================================================

        struct Node
        {
            int   value;
            Node* prev;
            Node* next;
        };

        // ==========================================================================
        // 迭代器（双向）
        // ==========================================================================

        class iterator
        {
        public:
            explicit iterator(Node* p = nullptr) : ptr_(p) {}

            int& operator*() const { return ptr_->value; }
            int* operator->() const { return &ptr_->value; }

            iterator& operator++()    { ptr_ = ptr_->next; return *this; }
            iterator  operator++(int) { iterator tmp = *this; ptr_ = ptr_->next; return tmp; }
            iterator& operator--()    { ptr_ = ptr_->prev; return *this; }
            iterator  operator--(int) { iterator tmp = *this; ptr_ = ptr_->prev; return tmp; }

            bool operator==(const iterator& o) const { return ptr_ == o.ptr_; }
            bool operator!=(const iterator& o) const { return ptr_ != o.ptr_; }

            Node* ptr() const { return ptr_; }

        private:
            Node* ptr_;
        };

        class const_iterator
        {
        public:
            explicit const_iterator(const Node* p = nullptr) : ptr_(p) {}

            const int& operator*() const { return ptr_->value; }
            const int* operator->() const { return &ptr_->value; }

            const_iterator& operator++()   { ptr_ = ptr_->next; return *this; }
            const_iterator  operator++(int){ const_iterator tmp = *this; ptr_ = ptr_->next; return tmp; }
            const_iterator& operator--()   { ptr_ = ptr_->prev; return *this; }
            const_iterator  operator--(int){ const_iterator tmp = *this; ptr_ = ptr_->prev; return tmp; }

            bool operator==(const const_iterator& o) const { return ptr_ == o.ptr_; }
            bool operator!=(const const_iterator& o) const { return ptr_ != o.ptr_; }

        private:
            const Node* ptr_;
        };

        // ==========================================================================
        // 构造 / 析构 / 赋值
        // ==========================================================================

        list() {
            sentinel_.prev = &sentinel_;
            sentinel_.next = &sentinel_;
            size_ = 0;
        }

        list(std::initializer_list<int> init) : list() {
            for (int v : init)
                push_back(v);
        }

        list(size_t count, int value = 0) : list() {
            for (size_t i = 0; i < count; ++i)
                push_back(value);
        }

        list(const list& other) : list() {
            for (const Node* n = other.sentinel_.next; n != &other.sentinel_; n = n->next)
                push_back(n->value);
        }

        list(list&& other) noexcept : list() {
            if (other.empty()) return;
            // 从 other 窃取所有节点
            Node* first = other.sentinel_.next;
            Node* last  = other.sentinel_.prev;
            // 接入自己的 sentinel
            sentinel_.next = first;
            sentinel_.prev = last;
            first->prev    = &sentinel_;
            last->next     = &sentinel_;
            size_ = other.size_;
            // 重置 other
            other.sentinel_.next = &other.sentinel_;
            other.sentinel_.prev = &other.sentinel_;
            other.size_ = 0;
        }

        list& operator=(const list& other) {
            if (this != &other) {
                list tmp(other);
                swap(tmp);
            }
            return *this;
        }

        list& operator=(list&& other) noexcept {
            if (this != &other) {
                clear();
                if (other.empty()) return *this;
                Node* first = other.sentinel_.next;
                Node* last  = other.sentinel_.prev;
                sentinel_.next = first;
                sentinel_.prev = last;
                first->prev    = &sentinel_;
                last->next     = &sentinel_;
                size_ = other.size_;
                other.sentinel_.next = &other.sentinel_;
                other.sentinel_.prev = &other.sentinel_;
                other.size_ = 0;
            }
            return *this;
        }

        ~list() { clear(); }

        // ==========================================================================
        // 迭代器
        // ==========================================================================

        iterator begin() { return iterator(sentinel_.next); }
        iterator end()   { return iterator(&sentinel_); }

        const_iterator begin() const { return const_iterator(sentinel_.next); }
        const_iterator end()   const { return const_iterator(&sentinel_); }

        // ==========================================================================
        // 元素访问
        // ==========================================================================

        int& front() { assert(!empty()); return sentinel_.next->value; }
        const int& front() const { assert(!empty()); return sentinel_.next->value; }

        int& back() { assert(!empty()); return sentinel_.prev->value; }
        const int& back() const { assert(!empty()); return sentinel_.prev->value; }

        // ==========================================================================
        // 容量
        // ==========================================================================

        size_t size()  const { return size_; }
        bool   empty() const { return size_ == 0; }

        // ==========================================================================
        // 修改器
        // ==========================================================================

        void push_back(const int& value)  { emplace_back(value); }
        void push_back(int&& value)       { emplace_back(std::move(value)); }
        void push_front(const int& value) { emplace_front(value); }
        void push_front(int&& value)      { emplace_front(std::move(value)); }

        template <typename... Args>
        void emplace_back(Args&&... args) {
            Node* n = new Node{int(std::forward<Args>(args)...), sentinel_.prev, &sentinel_};
            sentinel_.prev->next = n;
            sentinel_.prev = n;
            ++size_;
        }

        template <typename... Args>
        void emplace_front(Args&&... args) {
            Node* n = new Node{int(std::forward<Args>(args)...), &sentinel_, sentinel_.next};
            sentinel_.next->prev = n;
            sentinel_.next = n;
            ++size_;
        }

        void pop_back() {
            assert(!empty());
            Node* n = sentinel_.prev;
            sentinel_.prev = n->prev;
            n->prev->next = &sentinel_;
            delete n;
            --size_;
        }

        void pop_front() {
            assert(!empty());
            Node* n = sentinel_.next;
            sentinel_.next = n->next;
            n->next->prev = &sentinel_;
            delete n;
            --size_;
        }

        // insert 在 pos 之前插入，返回指向新元素的迭代器
        iterator insert(iterator pos, int value) {
            Node* at = pos.ptr();
            Node* n  = new Node{value, at->prev, at};
            at->prev->next = n;
            at->prev = n;
            ++size_;
            return iterator(n);
        }

        // erase 删除 pos 指向的元素，返回下一元素的迭代器
        iterator erase(iterator pos) {
            assert(!empty());
            Node* at = pos.ptr();
            assert(at != &sentinel_);
            Node* next = at->next;
            at->prev->next = at->next;
            at->next->prev = at->prev;
            delete at;
            --size_;
            return iterator(next);
        }

        void resize(size_t new_size, int value = 0) {
            while (size_ > new_size) pop_back();
            while (size_ < new_size) push_back(value);
        }

        void clear() {
            Node* cur = sentinel_.next;
            while (cur != &sentinel_) {
                Node* n = cur->next;
                delete cur;
                cur = n;
            }
            sentinel_.next = &sentinel_;
            sentinel_.prev = &sentinel_;
            size_ = 0;
        }

        void swap(list& other) {
            if (empty() && other.empty()) return;
            if (empty()) {
                // 把 other 的节点移过来
                Node* o_first = other.sentinel_.next;
                Node* o_last  = other.sentinel_.prev;
                sentinel_.next = o_first;
                sentinel_.prev = o_last;
                o_first->prev  = &sentinel_;
                o_last->next   = &sentinel_;
                other.sentinel_.next = &other.sentinel_;
                other.sentinel_.prev = &other.sentinel_;
            } else if (other.empty()) {
                Node* t_first = sentinel_.next;
                Node* t_last  = sentinel_.prev;
                other.sentinel_.next = t_first;
                other.sentinel_.prev = t_last;
                t_first->prev = &other.sentinel_;
                t_last->next  = &other.sentinel_;
                sentinel_.next = &sentinel_;
                sentinel_.prev = &sentinel_;
            } else {
                // 交换所有节点：交换两个 sentinel 的 next/prev 指向
                Node* t_first = sentinel_.next;
                Node* t_last  = sentinel_.prev;
                Node* o_first = other.sentinel_.next;
                Node* o_last  = other.sentinel_.prev;
                // 重新链接
                sentinel_.next = o_first;  sentinel_.prev = o_last;
                o_first->prev  = &sentinel_;  o_last->next   = &sentinel_;
                other.sentinel_.next = t_first;  other.sentinel_.prev = t_last;
                t_first->prev = &other.sentinel_;  t_last->next  = &other.sentinel_;
            }
            std::swap(size_, other.size_);
        }

        // ==========================================================================
        // list 专有操作
        // ==========================================================================

        void reverse() {
            if (size_ <= 1) return;
            Node* cur = sentinel_.next;
            while (cur != &sentinel_) {
                std::swap(cur->prev, cur->next);
                cur = cur->prev;   // prev 现在是原来的 next
            }
            // 交换 sentinel 的 next/prev
            std::swap(sentinel_.next, sentinel_.prev);
            // 修复与 sentinel 相邻的节点
            sentinel_.next->prev = &sentinel_;
            sentinel_.prev->next = &sentinel_;
        }

        void sort() {
            if (size_ <= 1) return;
            // 先从循环链中拆出所有节点，变为以 nullptr 结尾的线性链
            Node* head = sentinel_.next;
            Node* tail = sentinel_.prev;
            tail->next = nullptr;
            head->prev = nullptr;
            sentinel_.next = &sentinel_;
            sentinel_.prev = &sentinel_;

            head = merge_sort(head, size_);

            // 重新串回 sentinel（merge_sort 返回的链只有 next 正确）
            sentinel_.next = head;
            Node* cur = &sentinel_;
            while (cur->next) {
                cur->next->prev = cur;
                cur = cur->next;
            }
            sentinel_.prev = cur;
            cur->next = &sentinel_;
        }

        // 将 other 的所有元素拼接到 pos 之前，other 变空
        void splice(iterator pos, list& other) {
            if (other.empty()) return;
            Node* o_first = other.sentinel_.next;
            Node* o_last  = other.sentinel_.prev;
            // 从 other 中脱钩
            other.sentinel_.next = &other.sentinel_;
            other.sentinel_.prev = &other.sentinel_;
            // 接入
            Node* at = pos.ptr();
            o_last->next  = at;
            o_first->prev = at->prev;
            at->prev->next = o_first;
            at->prev       = o_last;
            size_ += other.size_;
            other.size_ = 0;
        }

    private:
        // ==========================================================================
        // 数据成员
        // ==========================================================================

        Node   sentinel_;   // 哨兵节点（value 不使用），next 指向头，prev 指向尾，一个哨兵节点同时充当头尾两种角色
        size_t size_;

        // 归并排序：对以 h 开头、长度为 len 的线性链表排序，返回新头
        static Node* merge_sort(Node* h, size_t len) {
            if (len <= 1) {
                if (h) h->next = nullptr;
                return h;
            }
            size_t mid = len / 2;
            Node* right     = h;
            Node* left_tail = nullptr;
            for (size_t i = 0; i < mid; ++i) {
                left_tail = right;
                right     = right->next;
            }
            if (left_tail) left_tail->next = nullptr;

            Node* left = merge_sort(h, mid);
            Node* r    = merge_sort(right, len - mid);
            return merge(left, r);
        }

        static Node* merge(Node* a, Node* b) {
            Node  dummy;
            Node* tail = &dummy;
            while (a && b) {
                if (a->value <= b->value) { tail->next = a; a = a->next; }
                else                      { tail->next = b; b = b->next; }
                tail = tail->next;
            }
            tail->next = a ? a : b;
            return dummy.next;
        }
    };

    // ============================================================================
    // 比较运算符
    // ============================================================================

    bool operator==(const list& a, const list& b) {
        if (a.size() != b.size()) return false;
        auto ia = a.begin(), ib = b.begin();
        for (; ia != a.end(); ++ia, ++ib) {
            if (*ia != *ib) return false;
        }
        return true;
    }

    bool operator!=(const list& a, const list& b) { return !(a == b); }

    bool operator<(const list& a, const list& b) {
        auto ia = a.begin(), ib = b.begin();
        for (; ia != a.end() && ib != b.end(); ++ia, ++ib) {
            if (*ia < *ib) return true;
            if (*ib < *ia) return false;
        }
        return a.size() < b.size();
    }

    bool operator>(const list& a, const list& b)  { return b < a; }
    bool operator<=(const list& a, const list& b) { return !(b < a); }
    bool operator>=(const list& a, const list& b) { return !(a < b); }

}  // namespace demo

// ============================================================================
// ============================================================================
// 测试函数
// ============================================================================
// ============================================================================

void TestConstruction()
{
    std::cout << "--- Testing construction ---" << std::endl;

    demo::list l1;
    std::cout << "Default: size=" << l1.size() << ", empty=" << l1.empty() << std::endl;

    demo::list l2(3, 10);
    std::cout << "Filled: ";
    for (auto it = l2.begin(); it != l2.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    demo::list l3 = l2;
    std::cout << "Copied: size=" << l3.size() << std::endl;

    demo::list l4(std::move(l2));
    std::cout << "Moved-from: size=" << l2.size() << ", empty=" << l2.empty() << std::endl;
    std::cout << "Moved-to: size=" << l4.size() << std::endl;

    demo::list l5;
    l5 = l3;
    std::cout << "Copy-assigned: size=" << l5.size() << std::endl;

    demo::list l6;
    l6 = std::move(l4);
    std::cout << "Move-assigned: size=" << l6.size()
              << ", moved-from size=" << l4.size() << std::endl;
}

void TestElementAccess()
{
    std::cout << "--- Testing element access ---" << std::endl;

    demo::list l;
    l.push_back(10);
    l.push_back(20);
    l.push_back(30);

    std::cout << "front: " << l.front() << std::endl;
    std::cout << "back: "  << l.back()  << std::endl;

    const demo::list& cl = l;
    std::cout << "const front: " << cl.front() << std::endl;
    std::cout << "const back: "  << cl.back()  << std::endl;
}

void TestPushPop()
{
    std::cout << "--- Testing push/pop ---" << std::endl;

    demo::list l;

    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    std::cout << "After push_back(1,2,3): ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    l.push_front(0);
    l.push_front(-1);
    std::cout << "After push_front(-1,0): ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    l.pop_back();
    std::cout << "After pop_back: ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << "| back=" << l.back() << std::endl;

    l.pop_front();
    std::cout << "After pop_front: ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << "| front=" << l.front() << std::endl;
}

void TestIterator()
{
    std::cout << "--- Testing iterator ---" << std::endl;

    demo::list l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    // 前向遍历
    std::cout << "Forward: ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    // 后置 ++
    auto it = l.begin();
    auto old = it++;
    std::cout << "Postfix++: *old=" << *old << ", *it=" << *it << std::endl;

    // 后置 --（从 end() 退回一步）
    it = l.end(); --it;
    old = it--;
    std::cout << "Postfix--: *old=" << *old << ", *it=" << *it << std::endl;

    // const 迭代器
    const demo::list& cl = l;
    std::cout << "Const iter: ";
    for (auto ci = cl.begin(); ci != cl.end(); ++ci) std::cout << *ci << " ";
    std::cout << std::endl;

    // 读写通过迭代器
    *l.begin() = 99;
    std::cout << "After write via iterator: front=" << l.front() << std::endl;
}

void TestInsertErase()
{
    std::cout << "--- Testing insert/erase ---" << std::endl;

    demo::list l;
    l.push_back(10);
    l.push_back(30);

    // insert 到中间
    auto it = l.begin();
    ++it;
    it = l.insert(it, 20);
    std::cout << "After insert 20: ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << "| inserted: " << *it << std::endl;

    // insert 到头部
    l.insert(l.begin(), 5);
    std::cout << "After insert 5 at begin: ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;

    // insert 到尾部
    l.insert(l.end(), 99);
    std::cout << "After insert 99 at end: ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;

    // erase 中间元素
    it = l.begin();
    ++it; ++it;   // 指向原来的 20
    it = l.erase(it);
    std::cout << "After erase 20, returned: " << *it << " | ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;

    // erase 头部
    l.erase(l.begin());
    std::cout << "After erase front: ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;

    // erase 尾部
    it = l.end(); --it;
    it = l.erase(it);
    std::cout << "After erase back, returned == end: " << (it == l.end()) << " | ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;
}

void TestResizeClear()
{
    std::cout << "--- Testing resize/clear ---" << std::endl;

    demo::list l;
    l.push_back(1);
    l.push_back(2);

    l.resize(5, 9);
    std::cout << "After resize(5, 9): size=" << l.size() << " | ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;

    l.resize(2);
    std::cout << "After resize(2): size=" << l.size() << " | ";
    for (auto x = l.begin(); x != l.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;

    l.clear();
    std::cout << "After clear: size=" << l.size() << ", empty=" << l.empty() << std::endl;
}

void TestComparisons()
{
    std::cout << "--- Testing comparisons ---" << std::endl;

    demo::list a;
    a.push_back(1); a.push_back(2); a.push_back(3);

    demo::list b;
    b.push_back(1); b.push_back(2); b.push_back(3);

    demo::list c;
    c.push_back(1); c.push_back(2); c.push_back(4);

    demo::list d;
    d.push_back(1); d.push_back(2);   // 比 a 短

    std::cout << "a == b: " << (a == b) << std::endl;
    std::cout << "a != c: " << (a != c) << std::endl;
    std::cout << "a < c:  " << (a < c)  << std::endl;
    std::cout << "c > a:  " << (c > a)  << std::endl;
    std::cout << "d < a:  " << (d < a)  << std::endl;
}

void TestSwap()
{
    std::cout << "--- Testing swap ---" << std::endl;

    demo::list a;
    a.push_back(1); a.push_back(2);

    demo::list b;
    b.push_back(100);

    std::cout << "Before swap: a.size=" << a.size() << ", b.size=" << b.size() << std::endl;
    a.swap(b);
    std::cout << "After swap: a.size=" << a.size() << ", b.size=" << b.size() << std::endl;
    std::cout << "a: ";
    for (auto it = a.begin(); it != a.end(); ++it) std::cout << *it << " ";
    std::cout << "| b: ";
    for (auto it = b.begin(); it != b.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    a.swap(b);
    std::cout << "Swapped back: a.front=" << a.front() << ", b.front=" << b.front() << std::endl;

    // 空列表 swap
    demo::list e;
    a.swap(e);
    std::cout << "Swap with empty: a.size=" << a.size() << ", e.size=" << e.size() << std::endl;
}

void TestReverse()
{
    std::cout << "--- Testing reverse ---" << std::endl;

    demo::list l;
    l.push_back(1); l.push_back(2); l.push_back(3); l.push_back(4); l.push_back(5);

    std::cout << "Before reverse: ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    l.reverse();
    std::cout << "After reverse:  ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << "| front=" << l.front() << ", back=" << l.back() << std::endl;

    // 验证双向完整性：反向遍历
    std::cout << "Reverse walk: ";
    auto it = l.end();
    while (it != l.begin()) {
        --it;
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void TestSort()
{
    std::cout << "--- Testing sort ---" << std::endl;

    demo::list l;
    l.push_back(3); l.push_back(1); l.push_back(4); l.push_back(1); l.push_back(5);
    l.push_back(9); l.push_back(2); l.push_back(6);

    std::cout << "Before sort: ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    l.sort();
    std::cout << "After sort:  ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    // 验证双向完整性
    std::cout << "Reverse check: ";
    auto it = l.end();
    while (it != l.begin()) {
        --it;
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // 单元素排序
    demo::list single;
    single.push_back(1);
    single.sort();
    std::cout << "Single sort: ok, size=" << single.size() << std::endl;

    // 空列表排序
    demo::list empty;
    empty.sort();
    std::cout << "Empty sort: ok" << std::endl;
}

void TestSplice()
{
    std::cout << "--- Testing splice ---" << std::endl;

    demo::list a;
    a.push_back(1); a.push_back(2); a.push_back(3);

    demo::list b;
    b.push_back(10); b.push_back(20); b.push_back(30);

    // splice 到末尾
    a.splice(a.end(), b);
    std::cout << "splice at end: a.size=" << a.size() << ", b.size=" << b.size() << std::endl;
    std::cout << "a: ";
    for (auto x = a.begin(); x != a.end(); ++x) std::cout << *x << " ";
    std::cout << "| b empty: " << b.empty() << std::endl;

    // splice 到中间
    demo::list c;
    c.push_back(100);

    auto it = a.begin();
    ++it; ++it; ++it;  // 指向第 4 个元素（10）
    a.splice(it, c);
    std::cout << "splice at middle: ";
    for (auto x = a.begin(); x != a.end(); ++x) std::cout << *x << " ";
    std::cout << "| c empty: " << c.empty() << std::endl;

    // splice 到空列表
    demo::list empty;
    empty.splice(empty.begin(), a);
    std::cout << "splice to empty: empty.size=" << empty.size() << ", a.size=" << a.size() << std::endl;
    std::cout << "empty: ";
    for (auto x = empty.begin(); x != empty.end(); ++x) std::cout << *x << " ";
    std::cout << std::endl;
}

void TestEmplace()
{
    std::cout << "--- Testing emplace ---" << std::endl;

    demo::list l;

    l.emplace_back(42);
    l.emplace_front(7);
    l.emplace_back(10 + 5);

    std::cout << "After emplace: ";
    for (auto it = l.begin(); it != l.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;
}

// ============================================================================
// main
// ============================================================================

int main()
{
    TestConstruction();
    TestElementAccess();
    TestPushPop();
    TestIterator();
    TestInsertErase();
    TestResizeClear();
    TestComparisons();
    TestSwap();
    TestReverse();
    TestSort();
    TestSplice();
    TestEmplace();

    std::cout << "\nAll list tests passed!" << std::endl;
    return 0;
}
