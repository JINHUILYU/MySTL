#ifndef MYSTL_LIST_H_
#define MYSTL_LIST_H_

#include <initializer_list>

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
#include "utility.h"

namespace mystl {

template <typename T, typename Alloc = allocator<T>>
class list {
    // ==========================================================================
    // Node type
    // ==========================================================================
    struct list_node {
        list_node* prev;
        list_node* next;
        T         value;
    };

    using node_alloc = typename allocator_traits<Alloc>::template rebind_alloc<list_node>;
    using node_alloc_traits = typename allocator_traits<Alloc>::template rebind_traits<list_node>::other;

public:
    // ==========================================================================
    // Type aliases
    // ==========================================================================
    using value_type      = T;
    using allocator_type  = Alloc;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = typename allocator_traits<Alloc>::pointer;
    using const_pointer   = typename allocator_traits<Alloc>::const_pointer;

    // ==========================================================================
    // Iterator (bidirectional)
    // ==========================================================================
    class iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T> {
    public:
        explicit iterator(list_node* p = nullptr) : ptr_(p) {}

        reference operator*() const { return ptr_->value; }
        pointer   operator->() const { return &ptr_->value; }

        iterator& operator++()    { ptr_ = ptr_->next; return *this; }
        iterator  operator++(int) { iterator tmp = *this; ptr_ = ptr_->next; return tmp; }
        iterator& operator--()    { ptr_ = ptr_->prev; return *this; }
        iterator  operator--(int) { iterator tmp = *this; ptr_ = ptr_->prev; return tmp; }

        bool operator==(const iterator& o) const { return ptr_ == o.ptr_; }
        bool operator!=(const iterator& o) const { return ptr_ != o.ptr_; }

        list_node* ptr() const { return ptr_; }

    private:
        list_node* ptr_;
    };

    class const_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T> {
    public:
        explicit const_iterator(const list_node* p = nullptr) : ptr_(p) {}
        const_iterator(const iterator& it) : ptr_(it.ptr()) {}

        const_reference operator*() const { return ptr_->value; }
        const_pointer   operator->() const { return &ptr_->value; }

        const_iterator& operator++()    { ptr_ = ptr_->next; return *this; }
        const_iterator  operator++(int) { const_iterator tmp = *this; ptr_ = ptr_->next; return tmp; }
        const_iterator& operator--()    { ptr_ = ptr_->prev; return *this; }
        const_iterator  operator--(int) { const_iterator tmp = *this; ptr_ = ptr_->prev; return tmp; }

        bool operator==(const const_iterator& o) const { return ptr_ == o.ptr_; }
        bool operator!=(const const_iterator& o) const { return ptr_ != o.ptr_; }

        const list_node* ptr() const { return ptr_; }

    private:
        const list_node* ptr_;
    };

    using reverse_iterator       = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    // ==========================================================================
    // Construction / destruction
    // ==========================================================================
    list() { init_sentinel(); }

    explicit list(size_type count, const T& value = T()) : list() {
        for (size_type i = 0; i < count; ++i) push_back(value);
    }

    template <typename InputIt, typename = enable_if_t<!is_integral<InputIt>::value>>
    list(InputIt first, InputIt last) : list() {
        for (auto it = first; it != last; ++it) push_back(*it);
    }

    list(const list& other) : list() {
        for (const list_node* n = other.sentinel_.next; n != &other.sentinel_; n = n->next)
            push_back(n->value);
    }

    list(list&& other) noexcept : list() {
        if (other.empty()) return;
        move_nodes_from(other);
        size_ = other.size_;
        other.size_ = 0;
    }

    list(std::initializer_list<T> init) : list() {
        for (const auto& elem : init) push_back(elem);
    }

    ~list() { clear(); }

    // ==========================================================================
    // Assignment
    // ==========================================================================
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
            move_nodes_from(other);
            size_ = other.size_;
            other.size_ = 0;
        }
        return *this;
    }

    list& operator=(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
        return *this;
    }

    // ==========================================================================
    // Element access
    // ==========================================================================
    reference       front()       { return sentinel_.next->value; }
    const_reference front() const { return sentinel_.next->value; }

    reference       back()       { return sentinel_.prev->value; }
    const_reference back() const { return sentinel_.prev->value; }

    // ==========================================================================
    // Capacity
    // ==========================================================================
    size_type size()  const noexcept { return size_; }
    bool      empty() const noexcept { return size_ == 0; }

    // ==========================================================================
    // Iterators
    // ==========================================================================
    iterator               begin()         noexcept { return iterator(sentinel_.next); }
    const_iterator         begin()   const noexcept { return const_iterator(sentinel_.next); }
    const_iterator         cbegin()  const noexcept { return const_iterator(sentinel_.next); }

    iterator               end()           noexcept { return iterator(&sentinel_); }
    const_iterator         end()     const noexcept { return const_iterator(&sentinel_); }
    const_iterator         cend()    const noexcept { return const_iterator(&sentinel_); }

    reverse_iterator       rbegin()        noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator       rend()          noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend()   const noexcept { return const_reverse_iterator(begin()); }

    // ==========================================================================
    // Modifiers
    // ==========================================================================
    void push_back(const T& value)  { emplace_back(value); }
    void push_back(T&& value)       { emplace_back(mystl::move(value)); }
    void push_front(const T& value) { emplace_front(value); }
    void push_front(T&& value)      { emplace_front(mystl::move(value)); }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        list_node* n = create_node(mystl::forward<Args>(args)...);
        link_before(&sentinel_, n);
        ++size_;
    }

    template <typename... Args>
    void emplace_front(Args&&... args) {
        list_node* n = create_node(mystl::forward<Args>(args)...);
        link_before(sentinel_.next, n);
        ++size_;
    }

    void pop_back() {
        list_node* n = sentinel_.prev;
        unlink(n);
        destroy_node(n);
        --size_;
    }

    void pop_front() {
        list_node* n = sentinel_.next;
        unlink(n);
        destroy_node(n);
        --size_;
    }

    iterator insert(iterator pos, const T& value) { return emplace(pos, value); }
    iterator insert(iterator pos, T&& value) { return emplace(pos, mystl::move(value)); }

    template <typename... Args>
    iterator emplace(iterator pos, Args&&... args) {
        list_node* n = create_node(mystl::forward<Args>(args)...);
        link_before(pos.ptr(), n);
        ++size_;
        return iterator(n);
    }

    iterator erase(iterator pos) {
        list_node* n = pos.ptr();
        iterator next_it(n->next);
        unlink(n);
        destroy_node(n);
        --size_;
        return next_it;
    }

    void resize(size_type new_size, const T& value = T()) {
        while (size_ > new_size) pop_back();
        while (size_ < new_size) push_back(value);
    }

    void clear() noexcept {
        list_node* cur = sentinel_.next;
        while (cur != &sentinel_) {
            list_node* n = cur->next;
            destroy_node(cur);
            cur = n;
        }
        sentinel_.next = &sentinel_;
        sentinel_.prev = &sentinel_;
        size_ = 0;
    }

    template <typename InputIt, typename = enable_if_t<!is_integral<InputIt>::value>>
    void assign(InputIt first, InputIt last) {
        clear();
        for (auto it = first; it != last; ++it) push_back(*it);
    }

    void assign(size_type count, const T& value) {
        clear();
        for (size_type i = 0; i < count; ++i) push_back(value);
    }

    void assign(std::initializer_list<T> init) { assign(init.begin(), init.end()); }

    void swap(list& other) noexcept {
        if (empty() && other.empty()) return;
        if (empty()) {
            move_nodes_from(other);
        } else if (other.empty()) {
            other.move_nodes_from(*this);
        } else {
            list_node* t_first = sentinel_.next;
            list_node* t_last  = sentinel_.prev;
            list_node* o_first = other.sentinel_.next;
            list_node* o_last  = other.sentinel_.prev;
            sentinel_.next = o_first;  sentinel_.prev = o_last;
            o_first->prev  = &sentinel_;  o_last->next  = &sentinel_;
            other.sentinel_.next = t_first;  other.sentinel_.prev = t_last;
            t_first->prev = &other.sentinel_;  t_last->next  = &other.sentinel_;
        }
        mystl::swap(size_, other.size_);
    }

    // ==========================================================================
    // List-specific operations
    // ==========================================================================
    void reverse() noexcept {
        if (size_ <= 1) return;
        list_node* cur = sentinel_.next;
        while (cur != &sentinel_) {
            mystl::swap(cur->prev, cur->next);
            cur = cur->prev;
        }
        mystl::swap(sentinel_.next, sentinel_.prev);
        sentinel_.next->prev = &sentinel_;
        sentinel_.prev->next = &sentinel_;
    }

    void sort() {
        if (size_ <= 1) return;
        list_node* head = sentinel_.next;
        list_node* tail = sentinel_.prev;
        tail->next = nullptr;
        head->prev = nullptr;
        sentinel_.next = &sentinel_;
        sentinel_.prev = &sentinel_;

        head = merge_sort(head, size_);

        sentinel_.next = head;
        list_node* cur = &sentinel_;
        while (cur->next) {
            cur->next->prev = cur;
            cur = cur->next;
        }
        sentinel_.prev = cur;
        cur->next = &sentinel_;
    }

    void splice(iterator pos, list& other) {
        if (other.empty()) return;
        list_node* o_first = other.sentinel_.next;
        list_node* o_last  = other.sentinel_.prev;
        other.sentinel_.next = &other.sentinel_;
        other.sentinel_.prev = &other.sentinel_;
        list_node* at = pos.ptr();
        o_last->next  = at;
        o_first->prev = at->prev;
        at->prev->next = o_first;
        at->prev       = o_last;
        size_ += other.size_;
        other.size_ = 0;
    }

    void splice(iterator pos, list&& other) { splice(pos, static_cast<list&>(other)); }

private:
    list_node sentinel_;
    size_type size_;
    allocator_type alloc_;

    void init_sentinel() {
        sentinel_.next = &sentinel_;
        sentinel_.prev = &sentinel_;
        size_ = 0;
    }

    template <typename... Args>
    list_node* create_node(Args&&... args) {
        node_alloc na(alloc_);
        list_node* n = node_alloc_traits::allocate(na, 1);
        try {
            allocator_traits<Alloc>::construct(alloc_, &n->value, mystl::forward<Args>(args)...);
        } catch (...) {
            node_alloc_traits::deallocate(na, n, 1);
            throw;
        }
        return n;
    }

    void destroy_node(list_node* n) {
        allocator_traits<Alloc>::destroy(alloc_, &n->value);
        node_alloc na(alloc_);
        node_alloc_traits::deallocate(na, n, 1);
    }

    static void link_before(list_node* pos, list_node* n) {
        n->next = pos;
        n->prev = pos->prev;
        pos->prev->next = n;
        pos->prev = n;
    }

    static void unlink(list_node* n) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    void move_nodes_from(list& other) {
        list_node* o_first = other.sentinel_.next;
        list_node* o_last  = other.sentinel_.prev;
        sentinel_.next = o_first;
        sentinel_.prev = o_last;
        o_first->prev = &sentinel_;
        o_last->next  = &sentinel_;
        other.sentinel_.next = &other.sentinel_;
        other.sentinel_.prev = &other.sentinel_;
    }

    static list_node* merge_sort(list_node* h, size_type len) {
        if (len <= 1) {
            if (h) h->next = nullptr;
            return h;
        }
        size_type mid = len / 2;
        list_node* right     = h;
        list_node* left_tail = nullptr;
        for (size_type i = 0; i < mid; ++i) {
            left_tail = right;
            right     = right->next;
        }
        if (left_tail) left_tail->next = nullptr;
        list_node* left = merge_sort(h, mid);
        list_node* r    = merge_sort(right, len - mid);
        return merge(left, r);
    }

    static list_node* merge(list_node* a, list_node* b) {
        list_node  dummy;
        list_node* tail = &dummy;
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
// Non-member comparison operators
// ============================================================================
template <typename T, typename Alloc>
bool operator==(const list<T, Alloc>& a, const list<T, Alloc>& b) {
    if (a.size() != b.size()) return false;
    auto ia = a.begin(), ib = b.begin();
    for (; ia != a.end(); ++ia, ++ib) {
        if (*ia != *ib) return false;
    }
    return true;
}

template <typename T, typename Alloc>
bool operator!=(const list<T, Alloc>& a, const list<T, Alloc>& b) {
    return !(a == b);
}

template <typename T, typename Alloc>
bool operator<(const list<T, Alloc>& a, const list<T, Alloc>& b) {
    auto ia = a.begin(), ib = b.begin();
    for (; ia != a.end() && ib != b.end(); ++ia, ++ib) {
        if (*ia < *ib) return true;
        if (*ib < *ia) return false;
    }
    return a.size() < b.size();
}

template <typename T, typename Alloc>
bool operator>(const list<T, Alloc>& a, const list<T, Alloc>& b) {
    return b < a;
}

template <typename T, typename Alloc>
bool operator<=(const list<T, Alloc>& a, const list<T, Alloc>& b) {
    return !(b < a);
}

template <typename T, typename Alloc>
bool operator>=(const list<T, Alloc>& a, const list<T, Alloc>& b) {
    return !(a < b);
}

template <typename T, typename Alloc>
void swap(list<T, Alloc>& a, list<T, Alloc>& b) noexcept {
    a.swap(b);
}

}  // namespace mystl

#endif  // MYSTL_LIST_H_
