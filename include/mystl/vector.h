#ifndef MYSTL_VECTOR_H_
#define MYSTL_VECTOR_H_

#include <initializer_list>

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
#include "utility.h"

namespace mystl {

template <typename T, typename Alloc = allocator<T>>
class vector {
public:
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = typename allocator_traits<Alloc>::pointer;
    using const_pointer = typename allocator_traits<Alloc>::const_pointer;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    // ==========================================================================
    // Construction / destruction
    // ==========================================================================

    vector() noexcept : begin_(nullptr), end_(nullptr), cap_(nullptr) {}  // 默认构造函数

    explicit vector(size_type count, const T& value = T()) : vector() { resize(count, value); }

    template <typename InputIt, typename = enable_if_t<!is_integral<InputIt>::value>>
    vector(InputIt first, InputIt last) : vector() {
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }

    vector(const vector& other) : vector() {
        reserve(other.size());
        for (const auto& elem : other) {
            push_back(elem);
        }
    }

    vector(vector&& other) noexcept : begin_(other.begin_), end_(other.end_), cap_(other.cap_) {
        other.begin_ = other.end_ = other.cap_ = nullptr;
    }

    vector(std::initializer_list<T> init) : vector() {
        reserve(init.size());
        for (const auto& elem : init) {
            push_back(elem);
        }
    }

    ~vector() {
        clear();
        allocator_traits<Alloc>::deallocate(alloc_, begin_, capacity());
    }

    // ==========================================================================
    // Assignment
    // ==========================================================================

    vector& operator=(const vector& other) {
        if (this != &other) {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            clear();
            allocator_traits<Alloc>::deallocate(alloc_, begin_, capacity());
            begin_ = other.begin_;
            end_ = other.end_;
            cap_ = other.cap_;
            other.begin_ = other.end_ = other.cap_ = nullptr;
        }
        return *this;
    }

    vector& operator=(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
        return *this;
    }

    // ==========================================================================
    // Element access
    // ==========================================================================

    reference operator[](size_type n) { return begin_[n]; }
    const_reference operator[](size_type n) const { return begin_[n]; }

    reference at(size_type n) {
        if (n >= size()) throw "vector::at: index out of range";
        return begin_[n];
    }
    const_reference at(size_type n) const {
        if (n >= size()) throw "vector::at: index out of range";
        return begin_[n];
    }

    reference front() { return *begin_; }
    const_reference front() const { return *begin_; }

    reference back() { return *(end_ - 1); }
    const_reference back() const { return *(end_ - 1); }

    T* data() noexcept { return begin_; }
    const T* data() const noexcept { return begin_; }

    // ==========================================================================
    // Capacity
    // ==========================================================================

    size_type size() const noexcept { return static_cast<size_type>(end_ - begin_); }
    size_type capacity() const noexcept { return static_cast<size_type>(cap_ - begin_); }
    bool empty() const noexcept { return begin_ == end_; }

    void reserve(size_type new_cap) {
        if (new_cap > capacity()) {
            reallocate(new_cap);
        }
    }

    void shrink_to_fit() {
        if (capacity() > size()) {
            reallocate(size());
        }
    }

    // ==========================================================================
    // Iterators
    // ==========================================================================

    iterator begin() noexcept { return begin_; }
    const_iterator begin() const noexcept { return begin_; }
    const_iterator cbegin() const noexcept { return begin_; }

    iterator end() noexcept { return end_; }
    const_iterator end() const noexcept { return end_; }
    const_iterator cend() const noexcept { return end_; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end_); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end_); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end_); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin_); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin_); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin_); }

    // ==========================================================================
    // Modifiers
    // ==========================================================================

    void push_back(const T& value) { emplace_back(value); }

    void push_back(T&& value) { emplace_back(move(value)); }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (end_ == cap_) {
            reserve(size() == 0 ? 4 : size() * 2);
        }
        allocator_traits<Alloc>::construct(alloc_, end_, mystl::forward<Args>(args)...);
        ++end_;
    }

    void pop_back() {
        --end_;
        allocator_traits<Alloc>::destroy(alloc_, end_);
    }

    iterator insert(const_iterator pos, const T& value) { return emplace(pos, value); }

    iterator insert(const_iterator pos, T&& value) { return emplace(pos, move(value)); }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        difference_type offset = pos - begin_;
        if (end_ == cap_) {
            reserve(size() == 0 ? 4 : size() * 2);
        }
        iterator p = begin_ + offset;

        if (p == end_) {
            allocator_traits<Alloc>::construct(alloc_, end_, mystl::forward<Args>(args)...);
            ++end_;
        } else {
            // Move the last element into uninitialized memory
            allocator_traits<Alloc>::construct(alloc_, end_, move(*(end_ - 1)));
            ++end_;

            // Shift elements right
            for (iterator it = end_ - 2; it > p; --it) {
                *it = move(*(it - 1));
            }

            // Destroy the old element at p and construct the new one
            *p = T(mystl::forward<Args>(args)...);
        }
        return p;
    }

    iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

    iterator erase(const_iterator first, const_iterator last) {
        iterator f = begin_ + (first - begin_);
        iterator l = begin_ + (last - begin_);

        if (f != l) {
            size_type n = static_cast<size_type>(end_ - l);
            for (size_type i = 0; i < n; ++i) {
                *(f + i) = move(*(l + i));
            }
            iterator new_end = f + n;
            while (end_ != new_end) {
                --end_;
                allocator_traits<Alloc>::destroy(alloc_, end_);
            }
        }
        return f;
    }

    void clear() noexcept {
        while (end_ != begin_) {
            --end_;
            allocator_traits<Alloc>::destroy(alloc_, end_);
        }
    }

    void resize(size_type count, const T& value = T()) {
        if (count < size()) {
            erase(begin_ + count, end_);
        } else if (count > size()) {
            reserve(count);
            for (size_type i = size(); i < count; ++i) {
                allocator_traits<Alloc>::construct(alloc_, end_, value);
                ++end_;
            }
        }
    }

    template <typename InputIt, typename = enable_if_t<!is_integral<InputIt>::value>>
    void assign(InputIt first, InputIt last) {
        clear();
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }

    void assign(size_type count, const T& value) {
        clear();
        reserve(count);
        for (size_type i = 0; i < count; ++i) {
            allocator_traits<Alloc>::construct(alloc_, end_, value);
            ++end_;
        }
    }

    void assign(std::initializer_list<T> init) { assign(init.begin(), init.end()); }

    void swap(vector& other) noexcept {
        mystl::swap(begin_, other.begin_);
        mystl::swap(end_, other.end_);
        mystl::swap(cap_, other.cap_);
    }

private:
    void reallocate(size_type new_cap) {
        pointer new_begin = allocator_traits<Alloc>::allocate(alloc_, new_cap);
        size_type old_size = size();

        if (is_nothrow_move_constructible<T>::value || !is_copy_constructible<T>::value) {
            for (size_type i = 0; i < old_size; ++i) {
                allocator_traits<Alloc>::construct(alloc_, new_begin + i, move(begin_[i]));
            }
        } else {
            for (size_type i = 0; i < old_size; ++i) {
                allocator_traits<Alloc>::construct(alloc_, new_begin + i, begin_[i]);
            }
        }

        for (size_type i = 0; i < old_size; ++i) {
            allocator_traits<Alloc>::destroy(alloc_, begin_ + i);
        }

        if (begin_) {
            allocator_traits<Alloc>::deallocate(alloc_, begin_, capacity());
        }

        begin_ = new_begin;
        end_ = new_begin + old_size;
        cap_ = new_begin + new_cap;
    }

    Alloc alloc_;
    pointer begin_;
    pointer end_;
    pointer cap_;
};

// ============================================================================
// Non-member comparison operators
// ============================================================================

template <typename T, typename Alloc>
bool operator==(const vector<T, Alloc>& a, const vector<T, Alloc>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

template <typename T, typename Alloc>
bool operator!=(const vector<T, Alloc>& a, const vector<T, Alloc>& b) {
    return !(a == b);
}

template <typename T, typename Alloc>
bool operator<(const vector<T, Alloc>& a, const vector<T, Alloc>& b) {
    size_t n = a.size() < b.size() ? a.size() : b.size();
    for (size_t i = 0; i < n; ++i) {
        if (a[i] < b[i]) return true;
        if (b[i] < a[i]) return false;
    }
    return a.size() < b.size();
}

template <typename T, typename Alloc>
bool operator>(const vector<T, Alloc>& a, const vector<T, Alloc>& b) {
    return b < a;
}

template <typename T, typename Alloc>
bool operator<=(const vector<T, Alloc>& a, const vector<T, Alloc>& b) {
    return !(b < a);
}

template <typename T, typename Alloc>
bool operator>=(const vector<T, Alloc>& a, const vector<T, Alloc>& b) {
    return !(a < b);
}

template <typename T, typename Alloc>
void swap(vector<T, Alloc>& a, vector<T, Alloc>& b) noexcept {
    a.swap(b);
}

}  // namespace mystl

#endif  // MYSTL_VECTOR_H_
