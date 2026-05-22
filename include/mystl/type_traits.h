#ifndef MYSTL_TYPE_TRAITS_H_
#define MYSTL_TYPE_TRAITS_H_

namespace mystl {

// ============================================================================
// integral_constant — the foundation of all type predicates
// ============================================================================

template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

template <bool B>
using bool_constant = integral_constant<bool, B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

// ============================================================================
// enable_if — SFINAE gate
// ============================================================================

template <bool, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
    using type = T;
};

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

// ============================================================================
// conditional — compile-time if
// ============================================================================

template <bool, typename T, typename>
struct conditional {
    using type = T;
};

template <typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

// ============================================================================
// is_same
// ============================================================================

template <typename T, typename U>
struct is_same : false_type {};

template <typename T>
struct is_same<T, T> : true_type {};

// ============================================================================
// remove_reference / add_lvalue_reference / add_rvalue_reference
// ============================================================================

template <typename T>
struct remove_reference {
    using type = T;
};
template <typename T>
struct remove_reference<T&> {
    using type = T;
};
template <typename T>
struct remove_reference<T&&> {
    using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

namespace detail {
template <typename T, bool = is_same<T, void>::value>
struct add_lvalue_reference_impl {
    using type = T&;
};
template <typename T>
struct add_lvalue_reference_impl<T, true> {
    using type_referable = T;
    using type = T;
};
}  // namespace detail

template <typename T>
struct add_lvalue_reference : detail::add_lvalue_reference_impl<T> {};

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

namespace detail {
template <typename T, bool = is_same<T, void>::value>
struct add_rvalue_reference_impl {
    using type = T&&;
};
template <typename T>
struct add_rvalue_reference_impl<T, true> {
    using type_referable = T;
    using type = T;
};
}  // namespace detail

template <typename T>
struct add_rvalue_reference : detail::add_rvalue_reference_impl<T> {};

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

// ============================================================================
// declval — used only in unevaluated contexts, never defined
// ============================================================================

template <typename T>
add_rvalue_reference_t<T> declval() noexcept;

// ============================================================================
// remove_cv / remove_const / remove_volatile
// ============================================================================

template <typename T>
struct remove_const {
    using type = T;
};
template <typename T>
struct remove_const<const T> {
    using type = T;
};
template <typename T>
using remove_const_t = typename remove_const<T>::type;

template <typename T>
struct remove_volatile {
    using type = T;
};
template <typename T>
struct remove_volatile<volatile T> {
    using type = T;
};
template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

template <typename T>
struct remove_cv {
    using type = remove_volatile_t<remove_const_t<T>>;
};
template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

// ============================================================================
// remove_extent (array → element type)
// ============================================================================

template <typename T>
struct remove_extent {
    using type = T;
};
template <typename T>
struct remove_extent<T[]> {
    using type = T;
};
template <typename T, size_t N>
struct remove_extent<T[N]> {
    using type = T;
};
template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

// ============================================================================
// remove_pointer / add_pointer
// ============================================================================

template <typename T>
struct remove_pointer {
    using type = T;
};
template <typename T>
struct remove_pointer<T*> {
    using type = T;
};
template <typename T>
struct remove_pointer<T* const> {
    using type = T;
};
template <typename T>
struct remove_pointer<T* volatile> {
    using type = T;
};
template <typename T>
struct remove_pointer<T* const volatile> {
    using type = T;
};
template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

namespace detail {
template <typename T, typename = void>
struct add_pointer_impl {
    using type = remove_reference_t<T>*;
};
template <typename T>
struct add_pointer_impl<T, enable_if_t<is_same<void, remove_cv_t<T>>::value>> {
    using type = T;
};
}  // namespace detail

template <typename T>
struct add_pointer : detail::add_pointer_impl<T> {};
template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

// ============================================================================
// is_void
// ============================================================================

template <typename T>
struct is_void : is_same<void, remove_cv_t<T>> {};

// ============================================================================
// is_null_pointer (C++14, but easy to add)
// ============================================================================

template <typename T>
struct is_null_pointer : is_same<decltype(nullptr), remove_cv_t<T>> {};

// ============================================================================
// is_integral
// ============================================================================

template <typename T>
struct is_integral : false_type {};
template <>
struct is_integral<bool> : true_type {};
template <>
struct is_integral<char> : true_type {};
template <>
struct is_integral<signed char> : true_type {};
template <>
struct is_integral<unsigned char> : true_type {};
template <>
struct is_integral<wchar_t> : true_type {};
template <>
struct is_integral<char16_t> : true_type {};
template <>
struct is_integral<char32_t> : true_type {};
template <>
struct is_integral<short> : true_type {};
template <>
struct is_integral<unsigned short> : true_type {};
template <>
struct is_integral<int> : true_type {};
template <>
struct is_integral<unsigned int> : true_type {};
template <>
struct is_integral<long> : true_type {};
template <>
struct is_integral<unsigned long> : true_type {};
template <>
struct is_integral<long long> : true_type {};
template <>
struct is_integral<unsigned long long> : true_type {};

// ============================================================================
// is_floating_point
// ============================================================================

template <typename T>
struct is_floating_point : false_type {};
template <>
struct is_floating_point<float> : true_type {};
template <>
struct is_floating_point<double> : true_type {};
template <>
struct is_floating_point<long double> : true_type {};

// ============================================================================
// is_array
// ============================================================================

template <typename T>
struct is_array : false_type {};
template <typename T>
struct is_array<T[]> : true_type {};
template <typename T, size_t N>
struct is_array<T[N]> : true_type {};

// ============================================================================
// is_pointer
// ============================================================================

template <typename T>
struct is_pointer : false_type {};
template <typename T>
struct is_pointer<T*> : true_type {};
template <typename T>
struct is_pointer<T* const> : true_type {};
template <typename T>
struct is_pointer<T* volatile> : true_type {};
template <typename T>
struct is_pointer<T* const volatile> : true_type {};

// ============================================================================
// is_reference / is_lvalue_reference / is_rvalue_reference
// ============================================================================

template <typename T>
struct is_lvalue_reference : false_type {};
template <typename T>
struct is_lvalue_reference<T&> : true_type {};

template <typename T>
struct is_rvalue_reference : false_type {};
template <typename T>
struct is_rvalue_reference<T&&> : true_type {};

template <typename T>
struct is_reference : false_type {};
template <typename T>
struct is_reference<T&> : true_type {};
template <typename T>
struct is_reference<T&&> : true_type {};

// ============================================================================
// is_const / is_volatile
// ============================================================================

template <typename T>
struct is_const : false_type {};
template <typename T>
struct is_const<const T> : true_type {};

template <typename T>
struct is_volatile : false_type {};
template <typename T>
struct is_volatile<volatile T> : true_type {};

// ============================================================================
// is_member_pointer
// ============================================================================

template <typename T>
struct is_member_pointer : false_type {};
template <typename T, typename U>
struct is_member_pointer<T U::*> : true_type {};

// ============================================================================
// is_function
// ============================================================================

template <typename T>
struct is_function : false_type {};

// Regular, const, volatile, const volatile, and ref-qualified variants
#define MYSTL_IS_FUNCTION_IMPL(CV)                          \
    template <typename R, typename... Args>                 \
    struct is_function<R(Args...) CV> : true_type {};       \
    template <typename R, typename... Args>                 \
    struct is_function<R(Args..., ...) CV> : true_type {};  \
    template <typename R, typename... Args>                 \
    struct is_function<R(Args...) CV&> : true_type {};      \
    template <typename R, typename... Args>                 \
    struct is_function<R(Args..., ...) CV&> : true_type {}; \
    template <typename R, typename... Args>                 \
    struct is_function<R(Args...) CV&&> : true_type {};     \
    template <typename R, typename... Args>                 \
    struct is_function<R(Args..., ...) CV&&> : true_type {};

MYSTL_IS_FUNCTION_IMPL()
MYSTL_IS_FUNCTION_IMPL(const)
MYSTL_IS_FUNCTION_IMPL(volatile)
MYSTL_IS_FUNCTION_IMPL(const volatile)

#undef MYSTL_IS_FUNCTION_IMPL

// ============================================================================
// Compiler-intrinsic dependent traits
// ============================================================================

template <typename T>
struct is_enum : integral_constant<bool, __is_enum(T)> {};
template <typename T>
struct is_union : integral_constant<bool, __is_union(T)> {};
template <typename T>
struct is_class : integral_constant<bool, __is_class(T)> {};

template <typename T>
struct is_abstract : integral_constant<bool, __is_abstract(T)> {};
template <typename T>
struct is_polymorphic : integral_constant<bool, __is_polymorphic(T)> {};
template <typename T>
struct is_empty : integral_constant<bool, __is_empty(T)> {};
template <typename T>
struct is_standard_layout : integral_constant<bool, __is_standard_layout(T)> {};
template <typename T>
struct is_pod : integral_constant<bool, __is_pod(T)> {};
template <typename T>
struct is_trivial : integral_constant<bool, __is_trivial(T)> {};

// is_trivially_* — use compiler builtins directly
template <typename T>
struct is_trivially_copyable : integral_constant<bool, __is_trivially_copyable(T)> {};
template <typename T>
struct is_trivially_constructible : integral_constant<bool, __is_trivially_constructible(T)> {};

// ============================================================================
// is_signed / is_unsigned / is_arithmetic / is_scalar / is_object / is_compound
// ============================================================================

template <typename T>
struct is_signed : integral_constant<bool, T(-1) < T(0)> {};

template <typename T>
struct is_arithmetic
    : integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value> {};

template <typename T>
struct is_unsigned : integral_constant<bool, !is_signed<T>::value && is_arithmetic<T>::value> {};

template <typename T>
struct is_fundamental : integral_constant<bool, is_arithmetic<T>::value || is_void<T>::value ||
                                                    is_null_pointer<T>::value> {};

template <typename T>
struct is_compound : integral_constant<bool, !is_fundamental<T>::value> {};

template <typename T>
struct is_scalar
    : integral_constant<bool, is_arithmetic<T>::value || is_enum<T>::value ||
                                  is_pointer<T>::value || is_member_pointer<T>::value ||
                                  is_null_pointer<T>::value> {};

template <typename T>
struct is_object : integral_constant<bool, is_scalar<T>::value || is_array<T>::value ||
                                               is_union<T>::value || is_class<T>::value> {};

// ============================================================================
// rank / extent — array introspection
// ============================================================================

template <typename T>
struct rank : integral_constant<size_t, 0> {};
template <typename T>
struct rank<T[]> : integral_constant<size_t, 1 + rank<T>::value> {};
template <typename T, size_t N>
struct rank<T[N]> : integral_constant<size_t, 1 + rank<T>::value> {};

template <typename T, unsigned I = 0>
struct extent : integral_constant<size_t, 0> {};
template <typename T>
struct extent<T[], 0> : integral_constant<size_t, 0> {};
template <typename T, unsigned I>
struct extent<T[], I> : integral_constant<size_t, extent<T, I - 1>::value> {};
template <typename T, size_t N>
struct extent<T[N], 0> : integral_constant<size_t, N> {};
template <typename T, size_t N, unsigned I>
struct extent<T[N], I> : integral_constant<size_t, extent<T, I - 1>::value> {};

// ============================================================================
// decay — the type resulting from pass-by-value
// ============================================================================

template <typename T>
struct decay {
private:
    using U = remove_reference_t<T>;

public:
    using type =
        conditional_t<is_array<U>::value, remove_extent_t<U>*,
                      conditional_t<is_function<U>::value, add_pointer_t<U>, remove_cv_t<U>>>;
};

template <typename T>
using decay_t = typename decay<T>::type;

// ============================================================================
// is_convertible
// ============================================================================

namespace detail {
template <typename From, typename To>
struct is_convertible_impl {
private:
    static void test(To);

    template <typename F, typename = decltype(test(declval<F>()))>
    static true_type check(int);

    template <typename>
    static false_type check(...);

public:
    using type = decltype(check<From>(0));
};
}  // namespace detail

template <typename From, typename To>
struct is_convertible : detail::is_convertible_impl<From, To>::type {};

// ============================================================================
// is_base_of
// ============================================================================

template <typename Base, typename Derived>
struct is_base_of : integral_constant<bool, __is_base_of(Base, Derived)> {};

// ============================================================================
// is_same decay test (doesn't require compiler support for is_base_of)
// (used as fallback; prefer __is_base_of)
// ============================================================================

// ============================================================================
// aligned_storage
// ============================================================================

template <size_t Len, size_t Align>
struct aligned_storage {
    struct type {
        alignas(Align) unsigned char _data[Len];
    };
};

template <size_t Len, size_t Align>
using aligned_storage_t = typename aligned_storage<Len, Align>::type;

// ============================================================================
// underlying_type
// ============================================================================

template <typename T>
struct underlying_type {
    using type = __underlying_type(T);
};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

// ============================================================================
// result_of (simplified) — deduces return type of a callable
// ============================================================================

namespace detail {
template <typename F, typename... Args>
auto result_of_impl(int) -> decltype(declval<F>()(declval<Args>()...));
template <typename, typename...>
auto result_of_impl(...) -> void;
}  // namespace detail

template <typename T>
struct result_of;

template <typename F, typename... Args>
struct result_of<F(Args...)> {
    using type = decltype(detail::result_of_impl<F, Args...>(0));
};

template <typename T>
using result_of_t = typename result_of<T>::type;

// ============================================================================
// is_constructible / is_default_constructible (compiler intrinsics)
// ============================================================================

template <typename T>
struct is_default_constructible : integral_constant<bool, __is_constructible(T)> {};

template <typename T>
struct is_copy_constructible : integral_constant<bool, __is_constructible(T, const T&)> {};

template <typename T>
struct is_move_constructible : integral_constant<bool, __is_constructible(T, T&&)> {};

template <typename T>
struct is_destructible : integral_constant<bool, __has_trivial_destructor(T) || true> {};

template <typename T>
struct is_nothrow_move_constructible : integral_constant<bool, __is_nothrow_constructible(T, T&&)> {
};

template <typename T>
struct is_nothrow_default_constructible : integral_constant<bool, __is_nothrow_constructible(T)> {};

// ============================================================================
// Convenience variable templates (C++14/17 style)
// ============================================================================

template <typename T>
constexpr bool is_void_v = is_void<T>::value;

template <typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template <typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

template <typename T>
constexpr bool is_array_v = is_array<T>::value;

template <typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;

template <typename T>
constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <typename T>
constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <typename T>
constexpr bool is_reference_v = is_reference<T>::value;

template <typename T>
constexpr bool is_const_v = is_const<T>::value;

template <typename T>
constexpr bool is_function_v = is_function<T>::value;

template <typename T>
constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

template <typename T>
constexpr bool is_fundamental_v = is_fundamental<T>::value;

template <typename T>
constexpr bool is_compound_v = is_compound<T>::value;

template <typename T>
constexpr bool is_scalar_v = is_scalar<T>::value;

template <typename T>
constexpr bool is_object_v = is_object<T>::value;

template <typename T>
constexpr bool is_enum_v = is_enum<T>::value;

template <typename T>
constexpr bool is_class_v = is_class<T>::value;

template <typename T>
constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

template <typename T>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

}  // namespace mystl

#endif  // MYSTL_TYPE_TRAITS_H_
