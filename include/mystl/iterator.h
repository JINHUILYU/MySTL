#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

#include <cstddef>  // ptrdiff_t

#include "type_traits.h"
#include "utility.h"

namespace mystl {

// ============================================================================
// Iterator category tags
// ============================================================================

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

// ============================================================================
// iterator — CRTP base for defining the five associated types
// ============================================================================

template <typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*,
          typename Reference = T&>
struct iterator {
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

// ============================================================================
// iterator_traits — extract iterator properties
// ============================================================================

template <typename Iter>
struct iterator_traits {
    using iterator_category = typename Iter::iterator_category;
    using value_type = typename Iter::value_type;
    using difference_type = typename Iter::difference_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
};

// Specialization for pointers
template <typename T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

// Specialization for const pointers
template <typename T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};

// ============================================================================
// reverse_iterator — adapts bidirectional / random-access iterators
// ============================================================================

template <typename Iter>
class reverse_iterator {
public:
    using iterator_type = Iter;
    using iterator_category = typename iterator_traits<Iter>::iterator_category;
    using value_type = typename iterator_traits<Iter>::value_type;
    using difference_type = typename iterator_traits<Iter>::difference_type;
    using pointer = typename iterator_traits<Iter>::pointer;
    using reference = typename iterator_traits<Iter>::reference;

    reverse_iterator() : current() {}
    explicit reverse_iterator(iterator_type x) : current(x) {}
    template <typename U>
    reverse_iterator(const reverse_iterator<U>& other) : current(other.base()) {}

    iterator_type base() const { return current; }

    reference operator*() const {
        Iter tmp = current;
        return *--tmp;
    }

    pointer operator->() const { return &(operator*()); }

    reverse_iterator& operator++() {
        --current;
        return *this;
    }

    reverse_iterator operator++(int) {
        reverse_iterator tmp = *this;
        --current;
        return tmp;
    }

    reverse_iterator& operator--() {
        ++current;
        return *this;
    }

    reverse_iterator operator--(int) {
        reverse_iterator tmp = *this;
        ++current;
        return tmp;
    }

    reverse_iterator operator+(difference_type n) const { return reverse_iterator(current - n); }

    reverse_iterator& operator+=(difference_type n) {
        current -= n;
        return *this;
    }

    reverse_iterator operator-(difference_type n) const { return reverse_iterator(current + n); }

    reverse_iterator& operator-=(difference_type n) {
        current += n;
        return *this;
    }

    reference operator[](difference_type n) const { return *(*this + n); }

protected:
    Iter current;
};

// Comparison operators
template <typename Iter1, typename Iter2>
bool operator==(const reverse_iterator<Iter1>& a, const reverse_iterator<Iter2>& b) {
    return a.base() == b.base();
}

template <typename Iter1, typename Iter2>
bool operator!=(const reverse_iterator<Iter1>& a, const reverse_iterator<Iter2>& b) {
    return a.base() != b.base();
}

template <typename Iter1, typename Iter2>
bool operator<(const reverse_iterator<Iter1>& a, const reverse_iterator<Iter2>& b) {
    return a.base() > b.base();
}

template <typename Iter1, typename Iter2>
bool operator>(const reverse_iterator<Iter1>& a, const reverse_iterator<Iter2>& b) {
    return a.base() < b.base();
}

template <typename Iter1, typename Iter2>
bool operator<=(const reverse_iterator<Iter1>& a, const reverse_iterator<Iter2>& b) {
    return a.base() >= b.base();
}

template <typename Iter1, typename Iter2>
bool operator>=(const reverse_iterator<Iter1>& a, const reverse_iterator<Iter2>& b) {
    return a.base() <= b.base();
}

template <typename Iter>
reverse_iterator<Iter> operator+(typename reverse_iterator<Iter>::difference_type n,
                                 const reverse_iterator<Iter>& it) {
    return it + n;
}

template <typename Iter1, typename Iter2>
auto operator-(const reverse_iterator<Iter1>& a,
               const reverse_iterator<Iter2>& b) -> decltype(b.base() - a.base()) {
    return b.base() - a.base();
}

// ============================================================================
// advance — move an iterator by n positions
// ============================================================================

namespace detail {

template <typename Iter>
void advance_impl(Iter& it, typename iterator_traits<Iter>::difference_type n, input_iterator_tag) {
    while (n > 0) {
        --n;
        ++it;
    }
}

template <typename Iter>
void advance_impl(Iter& it, typename iterator_traits<Iter>::difference_type n,
                  bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n--) ++it;
    } else {
        while (n++) --it;
    }
}

template <typename Iter>
void advance_impl(Iter& it, typename iterator_traits<Iter>::difference_type n,
                  random_access_iterator_tag) {
    it += n;
}

}  // namespace detail

template <typename Iter>
void advance(Iter& it, typename iterator_traits<Iter>::difference_type n) {
    detail::advance_impl(it, n, typename iterator_traits<Iter>::iterator_category());
}

// ============================================================================
// distance — number of increments from first to last
// ============================================================================

namespace detail {

template <typename Iter>
typename iterator_traits<Iter>::difference_type distance_impl(Iter first, Iter last,
                                                              input_iterator_tag) {
    typename iterator_traits<Iter>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template <typename Iter>
typename iterator_traits<Iter>::difference_type distance_impl(Iter first, Iter last,
                                                              random_access_iterator_tag) {
    return last - first;
}

}  // namespace detail

template <typename Iter>
typename iterator_traits<Iter>::difference_type distance(Iter first, Iter last) {
    return detail::distance_impl(first, last, typename iterator_traits<Iter>::iterator_category());
}

// ============================================================================
// next / prev — convenience helpers
// ============================================================================

template <typename Iter>
Iter next(Iter it, typename iterator_traits<Iter>::difference_type n = 1) {
    mystl::advance(it, n);
    return it;
}

template <typename Iter>
Iter prev(Iter it, typename iterator_traits<Iter>::difference_type n = 1) {
    mystl::advance(it, -n);
    return it;
}

}  // namespace mystl

#endif  // MYSTL_ITERATOR_H_
