#include <gtest/gtest.h>
#include <mystl/allocator.h>

using namespace mystl;

struct Counter {
    static int alive;
    int value;
    Counter() : value(0) { ++alive; }
    Counter(int v) : value(v) { ++alive; }
    ~Counter() { --alive; }
};
int Counter::alive = 0;

TEST(Allocator, allocate_deallocate) {
    allocator<int> alloc;
    int* p = alloc.allocate(5);
    EXPECT_NE(p, nullptr);
    alloc.deallocate(p, 5);
}

TEST(Allocator, construct_destroy) {
    allocator<Counter> alloc;
    Counter* p = alloc.allocate(1);
    ASSERT_EQ(Counter::alive, 0);

    alloc.construct(p, 42);
    EXPECT_EQ(Counter::alive, 1);
    EXPECT_EQ(p->value, 42);

    alloc.destroy(p);
    EXPECT_EQ(Counter::alive, 0);

    alloc.deallocate(p, 1);
}

TEST(Allocator, address) {
    allocator<int> alloc;
    int val = 10;
    EXPECT_EQ(alloc.address(val), &val);
    EXPECT_EQ(alloc.address(val), alloc.address(val));
}

TEST(Allocator, max_size) {
    allocator<int> alloc;
    EXPECT_GT(alloc.max_size(), 0u);
}

TEST(Allocator, equality) {
    allocator<int> a1, a2;
    EXPECT_TRUE(a1 == a2);
    EXPECT_FALSE(a1 != a2);

    allocator<double> a3;
    EXPECT_TRUE(a1 == a3);
}

TEST(Allocator, rebind) {
    bool same = (is_same<allocator<int>::rebind<double>::other, allocator<double>>::value);
    EXPECT_TRUE(same);
}

TEST(Allocator, allocator_traits_construct) {
    allocator<Counter> alloc;
    allocator_traits<allocator<Counter>> traits;

    Counter::alive = 0;
    auto* p = traits.allocate(alloc, 1);
    traits.construct(alloc, p, 100);

    EXPECT_EQ(Counter::alive, 1);
    EXPECT_EQ(p->value, 100);

    traits.destroy(alloc, p);
    traits.deallocate(alloc, p, 1);
    EXPECT_EQ(Counter::alive, 0);
}
