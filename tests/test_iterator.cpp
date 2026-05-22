#include <gtest/gtest.h>
#include <mystl/iterator.h>

#include <vector>  // for reference comparison only

using namespace mystl;

TEST(Iterator, iterator_traits_pointer) {
    using traits = iterator_traits<int*>;
    bool cat = (is_same<traits::iterator_category, random_access_iterator_tag>::value);
    EXPECT_TRUE(cat);
    EXPECT_TRUE((is_same<traits::value_type, int>::value));
    EXPECT_TRUE((is_same<traits::pointer, int*>::value));
    EXPECT_TRUE((is_same<traits::reference, int&>::value));
}

TEST(Iterator, iterator_traits_const_pointer) {
    using traits = iterator_traits<const int*>;
    EXPECT_TRUE((is_same<traits::value_type, int>::value));
    EXPECT_TRUE((is_same<traits::pointer, const int*>::value));
    EXPECT_TRUE((is_same<traits::reference, const int&>::value));
}

TEST(Iterator, iterator_tags_hierarchy) {
    // forward_iterator_tag IS-A input_iterator_tag
    EXPECT_TRUE((is_convertible<forward_iterator_tag*, input_iterator_tag*>::value));
    // bidirectional_iterator_tag IS-A forward_iterator_tag
    EXPECT_TRUE((is_convertible<bidirectional_iterator_tag*, forward_iterator_tag*>::value));
    // random_access_iterator_tag IS-A bidirectional_iterator_tag
    EXPECT_TRUE((is_convertible<random_access_iterator_tag*, bidirectional_iterator_tag*>::value));
}

TEST(Iterator, reverse_iterator_basics) {
    int arr[] = {1, 2, 3, 4, 5};
    reverse_iterator<int*> rit(arr + 5);  // points to past-the-end (arr+5)

    EXPECT_EQ(*rit, 5);
    ++rit;
    EXPECT_EQ(*rit, 4);
    --rit;
    EXPECT_EQ(*rit, 5);
}

TEST(Iterator, reverse_iterator_traversal) {
    int arr[] = {10, 20, 30};
    reverse_iterator<int*> rbegin(arr + 3);
    reverse_iterator<int*> rend(arr);

    EXPECT_EQ(*rbegin, 30);
    ++rbegin;
    EXPECT_EQ(*rbegin, 20);
    ++rbegin;
    EXPECT_EQ(*rbegin, 10);
    ++rbegin;
    EXPECT_TRUE(rbegin == rend);
}

TEST(Iterator, advance_input) {
    int arr[] = {1, 2, 3, 4, 5};
    int* p = arr;
    advance(p, 3);
    EXPECT_EQ(*p, 4);
}

TEST(Iterator, advance_random_access) {
    int arr[] = {1, 2, 3, 4, 5};
    int* p = arr + 4;
    advance(p, -2);
    EXPECT_EQ(*p, 3);
}

TEST(Iterator, distance_random_access) {
    int arr[] = {1, 2, 3, 4, 5};
    auto d = distance(arr, arr + 5);
    EXPECT_EQ(d, 5);
}

TEST(Iterator, next_prev) {
    int arr[] = {1, 2, 3};
    int* p = arr;
    auto q = next(p);
    EXPECT_EQ(*q, 2);
    auto r = next(p, 2);
    EXPECT_EQ(*r, 3);
    auto s = prev(r);
    EXPECT_EQ(*s, 2);
}
