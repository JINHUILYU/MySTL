#ifndef MYSTL_STRING_H_
#define MYSTL_STRING_H_

#include <initializer_list>

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
#include "utility.h"

namespace mystl {

// ============================================================================
// char_traits — character traits for basic_string (minimal implementation)
// ============================================================================

template <typename CharT>
struct char_traits {
    using char_type = CharT;
    using int_type = unsigned long;
    using pos_type = size_t;
    using off_type = ptrdiff_t;

    static void assign(char_type& c1, const char_type& c2) noexcept { c1 = c2; }
    static constexpr bool eq(char_type a, char_type b) noexcept { return a == b; }
    static constexpr bool lt(char_type a, char_type b) noexcept { return a < b; }

    static size_t length(const char_type* s) {
        size_t n = 0;
        while (!eq(*s++, char_type())) ++n;
        return n;
    }

    static int compare(const char_type* s1, const char_type* s2, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            if (lt(s1[i], s2[i])) return -1;
            if (lt(s2[i], s1[i])) return 1;
        }
        return 0;
    }

    static char_type* copy(char_type* dest, const char_type* src, size_t n) {
        for (size_t i = 0; i < n; ++i) assign(dest[i], src[i]);
        return dest;
    }

    static char_type* move(char_type* dest, const char_type* src, size_t n) {
        if (dest < src) {
            for (size_t i = 0; i < n; ++i) assign(dest[i], src[i]);
        } else if (dest > src) {
            for (size_t i = n; i > 0; --i) assign(dest[i - 1], src[i - 1]);
        }
        return dest;
    }

    static constexpr char_type to_char_type(int_type c) noexcept {
        return static_cast<char_type>(c);
    }

    static constexpr int_type to_int_type(char_type c) noexcept {
        return static_cast<int_type>(c);
    }

    static constexpr bool eq_int_type(int_type a, int_type b) noexcept { return a == b; }

    static constexpr int_type eof() noexcept { return static_cast<int_type>(-1); }

    static constexpr int_type not_eof(int_type c) noexcept {
        return eq_int_type(c, eof()) ? static_cast<int_type>(0) : c;
    }
};

// ============================================================================
// basic_string
// ============================================================================

template <typename CharT, typename Traits = char_traits<CharT>, typename Alloc = allocator<CharT>>
class basic_string {
public:
    using traits_type = Traits;
    using value_type = CharT;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = CharT&;
    using const_reference = const CharT&;
    using pointer = typename allocator_traits<Alloc>::pointer;
    using const_pointer = typename allocator_traits<Alloc>::const_pointer;
    using iterator = CharT*;
    using const_iterator = const CharT*;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    static constexpr size_type npos = static_cast<size_type>(-1);

    // ==========================================================================
    // Construction / destruction
    // ==========================================================================

    basic_string() noexcept : begin_(nullptr), end_(nullptr), cap_(nullptr) {
        begin_ = allocator_traits<Alloc>::allocate(alloc_, 1);
        end_ = begin_;
        cap_ = begin_;
        *end_ = CharT();
    }

    basic_string(size_type count, CharT ch) : basic_string() {
        resize(count, ch);
    }

    basic_string(const CharT* s) : basic_string() {
        size_type len = traits_type::length(s);
        reserve(len);
        for (size_type i = 0; i < len; ++i) {
            push_back(s[i]);
        }
    }

    basic_string(const basic_string& other) : basic_string() {
        reserve(other.size());
        for (size_type i = 0; i < other.size(); ++i) {
            push_back(other[i]);
        }
    }

    basic_string(basic_string&& other) noexcept
        : begin_(other.begin_), end_(other.end_), cap_(other.cap_) {
        other.begin_ = allocator_traits<Alloc>::allocate(other.alloc_, 1);
        other.end_ = other.begin_;
        other.cap_ = other.begin_;
        *other.end_ = CharT();
    }

    basic_string(const CharT* s, size_type count) : basic_string() {
        reserve(count);
        for (size_type i = 0; i < count; ++i) {
            push_back(s[i]);
        }
    }

    template <typename InputIt, typename = enable_if_t<!is_integral<InputIt>::value>>
    basic_string(InputIt first, InputIt last) : basic_string() {
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }

    basic_string(std::initializer_list<CharT> init) : basic_string() {
        reserve(init.size());
        for (auto ch : init) {
            push_back(ch);
        }
    }

    basic_string(const basic_string& other, size_type pos, size_type count = npos) : basic_string() {
        size_type len = other.size() - pos < count ? other.size() - pos : count;
        reserve(len);
        for (size_type i = 0; i < len; ++i) {
            push_back(other[pos + i]);
        }
    }

    ~basic_string() {
        clear();
        allocator_traits<Alloc>::deallocate(alloc_, begin_, capacity() + 1);
    }

    // ==========================================================================
    // Assignment
    // ==========================================================================

    basic_string& operator=(const basic_string& other) {
        if (this != &other) {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    basic_string& operator=(basic_string&& other) noexcept {
        if (this != &other) {
            clear();
            allocator_traits<Alloc>::deallocate(alloc_, begin_, capacity() + 1);
            begin_ = other.begin_;
            end_ = other.end_;
            cap_ = other.cap_;
            other.begin_ = allocator_traits<Alloc>::allocate(other.alloc_, 1);
            other.end_ = other.begin_;
            other.cap_ = other.begin_;
            *other.end_ = CharT();
        }
        return *this;
    }

    basic_string& operator=(const CharT* s) {
        assign(s);
        return *this;
    }

    basic_string& operator=(CharT ch) {
        assign(1, ch);
        return *this;
    }

    basic_string& operator=(std::initializer_list<CharT> init) {
        assign(init.begin(), init.end());
        return *this;
    }

    // ==========================================================================
    // Element access
    // ==========================================================================

    reference operator[](size_type n) { return begin_[n]; }
    const_reference operator[](size_type n) const { return begin_[n]; }

    reference at(size_type n) {
        if (n >= size()) throw "basic_string::at: index out of range";
        return begin_[n];
    }
    const_reference at(size_type n) const {
        if (n >= size()) throw "basic_string::at: index out of range";
        return begin_[n];
    }

    reference front() { return *begin_; }
    const_reference front() const { return *begin_; }

    reference back() { return *(end_ - 1); }
    const_reference back() const { return *(end_ - 1); }

    const CharT* c_str() const noexcept { return begin_; }
    const CharT* data() const noexcept { return begin_; }

    // ==========================================================================
    // Capacity
    // ==========================================================================

    size_type size() const noexcept { return static_cast<size_type>(end_ - begin_); }
    size_type length() const noexcept { return size(); }
    size_type capacity() const noexcept { return static_cast<size_type>(cap_ - begin_); }
    bool empty() const noexcept { return begin_ == end_; }

    size_type max_size() const noexcept { return static_cast<size_type>(-1) / sizeof(CharT) - 1; }

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

    void push_back(CharT ch) {
        if (end_ == cap_) {
            reserve(size() == 0 ? 4 : size() * 2);
        }
        allocator_traits<Alloc>::construct(alloc_, end_, ch);
        ++end_;
        *end_ = CharT();
    }

    void pop_back() {
        --end_;
        *end_ = CharT();
    }

    basic_string& append(size_type count, CharT ch) {
        reserve(size() + count);
        for (size_type i = 0; i < count; ++i) {
            allocator_traits<Alloc>::construct(alloc_, end_, ch);
            ++end_;
        }
        *end_ = CharT();
        return *this;
    }

    basic_string& append(const basic_string& other) {
        return append(other.data(), other.size());
    }

    basic_string& append(const CharT* s) { return append(s, traits_type::length(s)); }

    basic_string& append(const CharT* s, size_type count) {
        reserve(size() + count);
        for (size_type i = 0; i < count; ++i) {
            allocator_traits<Alloc>::construct(alloc_, end_, s[i]);
            ++end_;
        }
        *end_ = CharT();
        return *this;
    }

    basic_string& operator+=(const basic_string& other) { return append(other); }

    basic_string& operator+=(const CharT* s) { return append(s); }

    basic_string& operator+=(CharT ch) {
        push_back(ch);
        return *this;
    }

    void clear() noexcept {
        while (end_ != begin_) {
            --end_;
            allocator_traits<Alloc>::destroy(alloc_, end_);
        }
        *end_ = CharT();
    }

    basic_string& assign(size_type count, CharT ch) {
        clear();
        reserve(count);
        for (size_type i = 0; i < count; ++i) {
            allocator_traits<Alloc>::construct(alloc_, end_, ch);
            ++end_;
        }
        *end_ = CharT();
        return *this;
    }

    basic_string& assign(const basic_string& other) {
        assign(other.begin(), other.end());
        return *this;
    }

    basic_string& assign(const CharT* s) {
        assign(s, s + traits_type::length(s));
        return *this;
    }

    template <typename InputIt, typename = enable_if_t<!is_integral<InputIt>::value>>
    basic_string& assign(InputIt first, InputIt last) {
        clear();
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
        return *this;
    }

    basic_string& insert(size_type pos, size_type count, CharT ch) {
        insert(cbegin() + pos, count, ch);
        return *this;
    }

    basic_string& insert(size_type pos, const basic_string& other) {
        insert(cbegin() + pos, other.begin(), other.end());
        return *this;
    }

    iterator insert(const_iterator pos, CharT ch) { return insert(pos, 1, ch); }

    iterator insert(const_iterator pos, size_type count, CharT ch) {
        difference_type offset = pos - begin_;
        reserve(size() + count);
        iterator p = begin_ + offset;

        // Shift existing elements right
        size_type old_size = size();
        for (size_type i = old_size; i > static_cast<size_type>(offset); --i) {
            allocator_traits<Alloc>::construct(alloc_, begin_ + i + count - 1,
                                               mystl::move(begin_[i - 1]));
            allocator_traits<Alloc>::destroy(alloc_, begin_ + i - 1);
        }
        end_ += count;

        // Fill in the new chars
        for (size_type i = 0; i < count; ++i) {
            begin_[offset + i] = ch;
        }
        *end_ = CharT();
        return p;
    }

    template <typename InputIt, typename = enable_if_t<!is_integral<InputIt>::value>>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        difference_type offset = pos - begin_;
        size_type count = static_cast<size_type>(last - first);
        reserve(size() + count);
        iterator p = begin_ + offset;

        size_type old_size = size();
        for (size_type i = old_size; i > static_cast<size_type>(offset); --i) {
            allocator_traits<Alloc>::construct(alloc_, begin_ + i + count - 1,
                                               mystl::move(begin_[i - 1]));
            allocator_traits<Alloc>::destroy(alloc_, begin_ + i - 1);
        }
        end_ += count;

        for (size_type i = 0; i < count; ++i) {
            begin_[offset + i] = *first++;
        }
        *end_ = CharT();
        return p;
    }

    iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

    iterator erase(const_iterator first, const_iterator last) {
        iterator f = begin_ + (first - begin_);
        iterator l = begin_ + (last - begin_);

        if (f != l) {
            size_type n = static_cast<size_type>(end_ - l);
            for (size_type i = 0; i < n; ++i) {
                *(f + i) = mystl::move(*(l + i));
            }
            iterator new_end = f + n;
            while (end_ != new_end) {
                --end_;
                allocator_traits<Alloc>::destroy(alloc_, end_);
            }
            *end_ = CharT();
        }
        return f;
    }

    void resize(size_type count, CharT ch = CharT()) {
        if (count < size()) {
            erase(begin_ + count, end_);
        } else if (count > size()) {
            append(count - size(), ch);
        }
    }

    // ==========================================================================
    // String operations
    // ==========================================================================

    basic_string substr(size_type pos = 0, size_type count = npos) const {
        size_type len = size() - pos < count ? size() - pos : count;
        return basic_string(begin_ + pos, len);
    }

    size_type find(const basic_string& other, size_type pos = 0) const noexcept {
        return find(other.data(), pos, other.size());
    }

    size_type find(const CharT* s, size_type pos = 0) const noexcept {
        return find(s, pos, traits_type::length(s));
    }

    size_type find(const CharT* s, size_type pos, size_type count) const noexcept {
        if (count == 0) return pos <= size() ? pos : npos;
        if (pos + count > size()) return npos;

        for (size_type i = pos; i <= size() - count; ++i) {
            bool found = true;
            for (size_type j = 0; j < count; ++j) {
                if (!traits_type::eq(begin_[i + j], s[j])) {
                    found = false;
                    break;
                }
            }
            if (found) return i;
        }
        return npos;
    }

    size_type find(CharT ch, size_type pos = 0) const noexcept {
        for (size_type i = pos; i < size(); ++i) {
            if (traits_type::eq(begin_[i], ch)) return i;
        }
        return npos;
    }

    size_type rfind(const basic_string& other, size_type pos = npos) const noexcept {
        return rfind(other.data(), pos, other.size());
    }

    size_type rfind(const CharT* s, size_type pos = npos) const noexcept {
        return rfind(s, pos, traits_type::length(s));
    }

    size_type rfind(const CharT* s, size_type pos, size_type count) const noexcept {
        if (count == 0) return pos < size() ? pos : size();
        if (count > size()) return npos;

        size_type start = size() - count < pos ? size() - count : pos;
        for (size_type i = start + 1; i > 0; --i) {
            size_type idx = i - 1;
            bool found = true;
            for (size_type j = 0; j < count; ++j) {
                if (!traits_type::eq(begin_[idx + j], s[j])) {
                    found = false;
                    break;
                }
            }
            if (found) return idx;
        }
        return npos;
    }

    size_type rfind(CharT ch, size_type pos = npos) const noexcept {
        size_type start = size() > 0 ? (size() - 1 < pos ? size() - 1 : pos) : 0;
        for (size_type i = start + 1; i > 0; --i) {
            if (traits_type::eq(begin_[i - 1], ch)) return i - 1;
        }
        return npos;
    }

    size_type find_first_of(const CharT* s, size_type pos = 0) const noexcept {
        return find_first_of(s, pos, traits_type::length(s));
    }

    size_type find_first_of(const CharT* s, size_type pos, size_type count) const noexcept {
        for (size_type i = pos; i < size(); ++i) {
            for (size_type j = 0; j < count; ++j) {
                if (traits_type::eq(begin_[i], s[j])) return i;
            }
        }
        return npos;
    }

    size_type find_last_of(const CharT* s, size_type pos = npos) const noexcept {
        return find_last_of(s, pos, traits_type::length(s));
    }

    size_type find_last_of(const CharT* s, size_type pos, size_type count) const noexcept {
        if (size() == 0) return npos;
        size_type start = size() - 1 < pos ? size() - 1 : pos;
        for (size_type i = start + 1; i > 0; --i) {
            for (size_type j = 0; j < count; ++j) {
                if (traits_type::eq(begin_[i - 1], s[j])) return i - 1;
            }
        }
        return npos;
    }

    int compare(const basic_string& other) const noexcept {
        return compare(0, size(), other.data(), other.size());
    }

    int compare(size_type pos1, size_type count1, const basic_string& other) const noexcept {
        return compare(pos1, count1, other.data(), other.size());
    }

    int compare(size_type pos1, size_type count1, const CharT* s) const noexcept {
        return compare(pos1, count1, s, traits_type::length(s));
    }

    int compare(size_type pos1, size_type count1, const CharT* s, size_type count2) const noexcept {
        size_type rlen = size() - pos1 < count1 ? size() - pos1 : count1;
        int result = traits_type::compare(begin_ + pos1, s, rlen < count2 ? rlen : count2);
        if (result == 0) {
            if (rlen < count2) return -1;
            if (rlen > count2) return 1;
        }
        return result;
    }

    void swap(basic_string& other) noexcept {
        mystl::swap(begin_, other.begin_);
        mystl::swap(end_, other.end_);
        mystl::swap(cap_, other.cap_);
    }

private:
    void reallocate(size_type new_cap) {
        pointer new_begin = allocator_traits<Alloc>::allocate(alloc_, new_cap + 1);
        size_type old_size = size();

        if (mystl::is_nothrow_move_constructible<CharT>::value ||
            !mystl::is_copy_constructible<CharT>::value) {
            for (size_type i = 0; i < old_size; ++i) {
                allocator_traits<Alloc>::construct(alloc_, new_begin + i, mystl::move(begin_[i]));
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
            allocator_traits<Alloc>::deallocate(alloc_, begin_, capacity() + 1);
        }

        begin_ = new_begin;
        end_ = new_begin + old_size;
        cap_ = new_begin + new_cap;
        *end_ = CharT();
    }

    Alloc alloc_;
    pointer begin_;
    pointer end_;
    pointer cap_;
};

// ============================================================================
// Non-member comparison operators
// ============================================================================

template <typename CharT, typename Traits, typename Alloc>
bool operator==(const basic_string<CharT, Traits, Alloc>& a,
                const basic_string<CharT, Traits, Alloc>& b) {
    if (a.size() != b.size()) return false;
    return a.compare(b) == 0;
}

template <typename CharT, typename Traits, typename Alloc>
bool operator!=(const basic_string<CharT, Traits, Alloc>& a,
                const basic_string<CharT, Traits, Alloc>& b) {
    return !(a == b);
}

template <typename CharT, typename Traits, typename Alloc>
bool operator<(const basic_string<CharT, Traits, Alloc>& a,
               const basic_string<CharT, Traits, Alloc>& b) {
    return a.compare(b) < 0;
}

template <typename CharT, typename Traits, typename Alloc>
bool operator>(const basic_string<CharT, Traits, Alloc>& a,
               const basic_string<CharT, Traits, Alloc>& b) {
    return b < a;
}

template <typename CharT, typename Traits, typename Alloc>
bool operator<=(const basic_string<CharT, Traits, Alloc>& a,
                const basic_string<CharT, Traits, Alloc>& b) {
    return !(b < a);
}

template <typename CharT, typename Traits, typename Alloc>
bool operator>=(const basic_string<CharT, Traits, Alloc>& a,
                const basic_string<CharT, Traits, Alloc>& b) {
    return !(a < b);
}

// ============================================================================
// operator+
// ============================================================================

template <typename CharT, typename Traits, typename Alloc>
basic_string<CharT, Traits, Alloc> operator+(const basic_string<CharT, Traits, Alloc>& a,
                                              const basic_string<CharT, Traits, Alloc>& b) {
    basic_string<CharT, Traits, Alloc> result(a);
    result.append(b);
    return result;
}

template <typename CharT, typename Traits, typename Alloc>
basic_string<CharT, Traits, Alloc> operator+(const CharT* a,
                                              const basic_string<CharT, Traits, Alloc>& b) {
    basic_string<CharT, Traits, Alloc> result(a);
    result.append(b);
    return result;
}

template <typename CharT, typename Traits, typename Alloc>
basic_string<CharT, Traits, Alloc> operator+(const basic_string<CharT, Traits, Alloc>& a,
                                              const CharT* b) {
    basic_string<CharT, Traits, Alloc> result(a);
    result.append(b);
    return result;
}

template <typename CharT, typename Traits, typename Alloc>
basic_string<CharT, Traits, Alloc> operator+(CharT a,
                                              const basic_string<CharT, Traits, Alloc>& b) {
    basic_string<CharT, Traits, Alloc> result(1, a);
    result.append(b);
    return result;
}

template <typename CharT, typename Traits, typename Alloc>
basic_string<CharT, Traits, Alloc> operator+(const basic_string<CharT, Traits, Alloc>& a, CharT b) {
    basic_string<CharT, Traits, Alloc> result(a);
    result.push_back(b);
    return result;
}

template <typename CharT, typename Traits, typename Alloc>
void swap(basic_string<CharT, Traits, Alloc>& a,
          basic_string<CharT, Traits, Alloc>& b) noexcept {
    a.swap(b);
}

// ============================================================================
// Convenience typedefs
// ============================================================================

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

}  // namespace mystl

#endif  // MYSTL_STRING_H_
