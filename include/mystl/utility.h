#ifndef MYSTL_UTILITY_H_
#define MYSTL_UTILITY_H_

#include "type_traits.h"

namespace mystl {

// ============================================================================
// move — cast to rvalue reference
// ============================================================================

template <typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
}

// ============================================================================
// forward — perfect forwarding
// ============================================================================

template <typename T>
constexpr T&& forward(remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>&& t) noexcept {
    static_assert(!is_lvalue_reference<T>::value, "cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(t);
}

// ============================================================================
// swap
// ============================================================================

template <typename T>
void swap(T& a, T& b) noexcept(is_nothrow_move_constructible<T>::value) {
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}

template <typename T, size_t N>
void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(swap(*a, *b))) {
    for (size_t i = 0; i < N; ++i) {
        swap(a[i], b[i]);
    }
}

// ============================================================================
// pair
// ============================================================================

template <typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    pair() : first(), second() {}

    pair(const T1& x, const T2& y) : first(x), second(y) {}

    template <typename U1, typename U2>
    pair(const pair<U1, U2>& other) : first(other.first), second(other.second) {}

    template <typename U1, typename U2>
    pair(pair<U1, U2>&& other) : first(move(other.first)), second(move(other.second)) {}

    pair(const pair&) = default;
    pair(pair&&) = default;

    pair& operator=(const pair& other) {
        first = other.first;
        second = other.second;
        return *this;
    }

    pair& operator=(pair&& other) {
        first = move(other.first);
        second = move(other.second);
        return *this;
    }

    void swap(pair& other) noexcept(noexcept(mystl::swap(first, other.first)) &&
                                    noexcept(mystl::swap(second, other.second))) {
        mystl::swap(first, other.first);
        mystl::swap(second, other.second);
    }
};

// ============================================================================
// pair comparisons
// ============================================================================

template <typename T1, typename T2>
bool operator==(const pair<T1, T2>& a, const pair<T1, T2>& b) {
    return a.first == b.first && a.second == b.second;
}

template <typename T1, typename T2>
bool operator!=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
    return !(a == b);
}

template <typename T1, typename T2>
bool operator<(const pair<T1, T2>& a, const pair<T1, T2>& b) {
    return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
}

template <typename T1, typename T2>
bool operator>(const pair<T1, T2>& a, const pair<T1, T2>& b) {
    return b < a;
}

template <typename T1, typename T2>
bool operator<=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
    return !(b < a);
}

template <typename T1, typename T2>
bool operator>=(const pair<T1, T2>& a, const pair<T1, T2>& b) {
    return !(a < b);
}

// ============================================================================
// swap overload for pair
// ============================================================================

template <typename T1, typename T2>
void swap(pair<T1, T2>& a, pair<T1, T2>& b) noexcept(noexcept(a.swap(b))) {
    a.swap(b);
}

// ============================================================================
// make_pair
// ============================================================================

template <typename T1, typename T2>
pair<decay_t<T1>, decay_t<T2>> make_pair(T1&& x, T2&& y) {
    return pair<decay_t<T1>, decay_t<T2>>(forward<T1>(x), forward<T2>(y));
}

}  // namespace mystl

#endif  // MYSTL_UTILITY_H_
