#include <gtest/gtest.h>
#include <mystl/type_traits.h>

using namespace mystl;

// integral_constant
TEST(TypeTraits, integral_constant) {
    using two = integral_constant<int, 2>;
    EXPECT_EQ(two::value, 2);
    EXPECT_EQ(two()(), 2);

    EXPECT_TRUE(true_type::value);
    EXPECT_FALSE(false_type::value);
}

// enable_if
TEST(TypeTraits, enable_if) {
    using t = enable_if<true, int>::type;
    EXPECT_TRUE((is_same<t, int>::value));

    // enable_if<false, int> has no ::type — this test just compiles
    EXPECT_TRUE((is_same<enable_if_t<true, double>, double>::value));
}

// conditional
TEST(TypeTraits, conditional) {
    EXPECT_TRUE((is_same<conditional_t<true, int, float>, int>::value));
    EXPECT_TRUE((is_same<conditional_t<false, int, float>, float>::value));
}

// is_same
TEST(TypeTraits, is_same) {
    EXPECT_TRUE((is_same<int, int>::value));
    EXPECT_FALSE((is_same<int, float>::value));
    EXPECT_FALSE((is_same<int, const int>::value));
}

// remove_reference
TEST(TypeTraits, remove_reference) {
    EXPECT_TRUE((is_same<remove_reference_t<int>, int>::value));
    EXPECT_TRUE((is_same<remove_reference_t<int&>, int>::value));
    EXPECT_TRUE((is_same<remove_reference_t<int&&>, int>::value));
}

// add_lvalue_reference
TEST(TypeTraits, add_lvalue_reference) {
    EXPECT_TRUE((is_same<add_lvalue_reference_t<int>, int&>::value));
    EXPECT_TRUE((is_same<add_lvalue_reference_t<int&>, int&>::value));
    // void stays void
    EXPECT_TRUE((is_same<add_lvalue_reference_t<void>, void>::value));
}

// add_rvalue_reference
TEST(TypeTraits, add_rvalue_reference) {
    EXPECT_TRUE((is_same<add_rvalue_reference_t<int>, int&&>::value));
    EXPECT_TRUE((is_same<add_rvalue_reference_t<int&>, int&>::value));
    EXPECT_TRUE((is_same<add_rvalue_reference_t<void>, void>::value));
}

// remove_cv
TEST(TypeTraits, remove_cv) {
    EXPECT_TRUE((is_same<remove_const_t<const int>, int>::value));
    EXPECT_TRUE((is_same<remove_volatile_t<volatile int>, int>::value));
    EXPECT_TRUE((is_same<remove_cv_t<const volatile int>, int>::value));
    EXPECT_TRUE((is_same<remove_cv_t<int>, int>::value));
}

// remove_extent
TEST(TypeTraits, remove_extent) {
    EXPECT_TRUE((is_same<remove_extent_t<int[]>, int>::value));
    EXPECT_TRUE((is_same<remove_extent_t<int[5]>, int>::value));
    EXPECT_TRUE((is_same<remove_extent_t<int>, int>::value));
}

// remove_pointer / add_pointer
TEST(TypeTraits, remove_pointer) {
    EXPECT_TRUE((is_same<remove_pointer_t<int*>, int>::value));
    EXPECT_TRUE((is_same<remove_pointer_t<int* const>, int>::value));
    EXPECT_TRUE((is_same<remove_pointer_t<int>, int>::value));
}

TEST(TypeTraits, add_pointer) {
    EXPECT_TRUE((is_same<add_pointer_t<int>, int*>::value));
    EXPECT_TRUE((is_same<add_pointer_t<int&>, int*>::value));
}

// is_void
TEST(TypeTraits, is_void) {
    EXPECT_TRUE(is_void<void>::value);
    EXPECT_TRUE(is_void<const void>::value);
    EXPECT_FALSE(is_void<int>::value);
}

// is_integral
TEST(TypeTraits, is_integral) {
    EXPECT_TRUE(is_integral<int>::value);
    EXPECT_TRUE(is_integral<bool>::value);
    EXPECT_TRUE(is_integral<char>::value);
    EXPECT_TRUE(is_integral<unsigned long long>::value);
    EXPECT_FALSE(is_integral<float>::value);
    EXPECT_FALSE(is_integral<int*>::value);
}

// is_floating_point
TEST(TypeTraits, is_floating_point) {
    EXPECT_TRUE(is_floating_point<float>::value);
    EXPECT_TRUE(is_floating_point<double>::value);
    EXPECT_TRUE(is_floating_point<long double>::value);
    EXPECT_FALSE(is_floating_point<int>::value);
}

// is_array
TEST(TypeTraits, is_array) {
    EXPECT_TRUE(is_array<int[]>::value);
    EXPECT_TRUE((is_array<int[5]>::value));
    EXPECT_FALSE(is_array<int>::value);
    EXPECT_FALSE(is_array<int*>::value);
}

// is_pointer
TEST(TypeTraits, is_pointer) {
    EXPECT_TRUE(is_pointer<int*>::value);
    EXPECT_TRUE(is_pointer<int* const>::value);
    EXPECT_FALSE(is_pointer<int>::value);
    EXPECT_FALSE(is_pointer<int&>::value);
}

// is_reference
TEST(TypeTraits, is_reference) {
    EXPECT_TRUE(is_reference<int&>::value);
    EXPECT_TRUE(is_reference<int&&>::value);
    EXPECT_FALSE(is_reference<int>::value);

    EXPECT_TRUE(is_lvalue_reference<int&>::value);
    EXPECT_FALSE(is_lvalue_reference<int&&>::value);

    EXPECT_FALSE(is_rvalue_reference<int&>::value);
    EXPECT_TRUE(is_rvalue_reference<int&&>::value);
}

// is_const / is_volatile
TEST(TypeTraits, is_const) {
    EXPECT_TRUE(is_const<const int>::value);
    EXPECT_TRUE(is_const<const volatile int>::value);
    EXPECT_FALSE(is_const<int>::value);
}

TEST(TypeTraits, is_volatile) {
    EXPECT_TRUE(is_volatile<volatile int>::value);
    EXPECT_FALSE(is_volatile<int>::value);
}

// is_function
TEST(TypeTraits, is_function) {
    EXPECT_TRUE(is_function<void()>::value);
    EXPECT_TRUE(is_function<int(float, double)>::value);
    EXPECT_TRUE((is_function<void() const>::value));
    EXPECT_FALSE(is_function<int>::value);
    EXPECT_FALSE(is_function<int*>::value);
}

// is_signed / is_unsigned
TEST(TypeTraits, is_signed) {
    EXPECT_TRUE(is_signed<int>::value);
    EXPECT_TRUE(is_signed<float>::value);
    EXPECT_FALSE(is_signed<unsigned int>::value);
}

// is_arithmetic / is_fundamental / is_compound
TEST(TypeTraits, category) {
    EXPECT_TRUE(is_arithmetic<int>::value);
    EXPECT_TRUE(is_arithmetic<double>::value);
    EXPECT_FALSE(is_arithmetic<int*>::value);

    EXPECT_TRUE(is_fundamental<int>::value);
    EXPECT_TRUE(is_fundamental<void>::value);
    EXPECT_FALSE(is_fundamental<int*>::value);

    EXPECT_FALSE(is_compound<int>::value);
    EXPECT_TRUE(is_compound<int*>::value);
}

// rank / extent
TEST(TypeTraits, rank) {
    EXPECT_EQ(rank<int>::value, 0u);
    EXPECT_EQ(rank<int[]>::value, 1u);
    EXPECT_EQ((rank<int[5]>::value), 1u);
    EXPECT_EQ((rank<int[5][10]>::value), 2u);
}

TEST(TypeTraits, extent) {
    EXPECT_EQ((extent<int[5]>::value), 5u);
    EXPECT_EQ((extent<int[5][10], 1>::value), 10u);
    EXPECT_EQ((extent<int>::value), 0u);
}

// decay
TEST(TypeTraits, decay) {
    EXPECT_TRUE((is_same<decay_t<int>, int>::value));
    EXPECT_TRUE((is_same<decay_t<int&>, int>::value));
    EXPECT_TRUE((is_same<decay_t<int&&>, int>::value));
    EXPECT_TRUE((is_same<decay_t<const int&>, int>::value));
    EXPECT_TRUE((is_same<decay_t<int[5]>, int*>::value));
    EXPECT_TRUE((is_same<decay_t<int(int)>, int (*)(int)>::value));
}

// is_convertible
TEST(TypeTraits, is_convertible) {
    EXPECT_TRUE((is_convertible<int, double>::value));
    EXPECT_TRUE((is_convertible<int, long>::value));
    EXPECT_TRUE((is_convertible<const char*, const void*>::value));
    EXPECT_FALSE((is_convertible<int*, float*>::value));
}

// is_base_of
struct Base {};
struct Derived : Base {};
struct Unrelated {};

TEST(TypeTraits, is_base_of) {
    EXPECT_TRUE((is_base_of<Base, Derived>::value));
    EXPECT_TRUE((is_base_of<Base, Base>::value));
    EXPECT_FALSE((is_base_of<Derived, Base>::value));
    EXPECT_FALSE((is_base_of<Base, Unrelated>::value));
}

// is_enum / is_class / is_union
enum Color { Red, Green, Blue };
enum class Direction { North, South };

TEST(TypeTraits, is_enum) {
    EXPECT_TRUE(is_enum<Color>::value);
    EXPECT_TRUE(is_enum<Direction>::value);
    EXPECT_FALSE(is_enum<int>::value);
}

TEST(TypeTraits, is_class) {
    EXPECT_TRUE(is_class<Base>::value);
    EXPECT_FALSE(is_class<int>::value);
}
