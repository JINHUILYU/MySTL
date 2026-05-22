#include <gtest/gtest.h>
#include <mystl/vector.h>

using namespace mystl;

struct Tracked {
    static int alive;
    static int copies;
    static int moves;
    int val;

    Tracked() : val(0) { ++alive; }
    explicit Tracked(int v) : val(v) { ++alive; }
    Tracked(const Tracked& o) : val(o.val) {
        ++alive;
        ++copies;
    }
    Tracked(Tracked&& o) noexcept : val(o.val) {
        o.val = 0;
        ++alive;
        ++moves;
    }
    ~Tracked() { --alive; }

    Tracked& operator=(const Tracked& o) {
        if (this != &o) val = o.val;
        return *this;
    }
    Tracked& operator=(Tracked&& o) noexcept {
        val = o.val;
        o.val = 0;
        return *this;
    }

    static void reset() { alive = copies = moves = 0; }
};
int Tracked::alive = 0;
int Tracked::copies = 0;
int Tracked::moves = 0;

// ============================================================================
// Construction
// ============================================================================

TEST(Vector, default_construct) {
    vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
}

TEST(Vector, size_count_construct) {
    vector<int> v(5, 42);
    EXPECT_EQ(v.size(), 5u);
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], 42);
    }
}

TEST(Vector, default_value_construct) {
    vector<int> v(3);
    EXPECT_EQ(v.size(), 3u);
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], 0);
    }
}

TEST(Vector, copy_construct) {
    vector<int> v1(3, 10);
    vector<int> v2(v1);
    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2[0], 10);
    EXPECT_EQ(v2[1], 10);
    EXPECT_EQ(v2[2], 10);
}

TEST(Vector, move_construct) {
    vector<int> v1(3, 10);
    vector<int> v2(move(v1));
    EXPECT_EQ(v2.size(), 3u);
    EXPECT_TRUE(v1.empty());
}

TEST(Vector, initializer_list) {
    vector<int> v = {1, 2, 3, 4};
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[3], 4);
}

TEST(Vector, range_construct) {
    int arr[] = {10, 20, 30};
    vector<int> v(arr, arr + 3);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[1], 20);
}

// ============================================================================
// Element access
// ============================================================================

TEST(Vector, element_access) {
    vector<int> v = {10, 20, 30};
    EXPECT_EQ(v.front(), 10);
    EXPECT_EQ(v.back(), 30);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v.at(0), 10);
    EXPECT_THROW(v.at(5), const char*);
    EXPECT_EQ(v.data()[2], 30);
}

// ============================================================================
// Capacity
// ============================================================================

TEST(Vector, reserve) {
    vector<int> v;
    v.reserve(100);
    EXPECT_GE(v.capacity(), 100u);
    EXPECT_EQ(v.size(), 0u);
}

TEST(Vector, shrink_to_fit) {
    vector<int> v;
    v.reserve(100);
    v.push_back(1);
    v.push_back(2);
    size_t old_cap = v.capacity();
    EXPECT_GE(old_cap, 100u);
    v.shrink_to_fit();
    EXPECT_LE(v.capacity(), old_cap);
}

// ============================================================================
// Modifiers
// ============================================================================

TEST(Vector, push_back) {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[2], 3);
}

TEST(Vector, emplace_back) {
    vector<Tracked> v;
    Tracked::reset();

    v.emplace_back(10);
    EXPECT_EQ(v.size(), 1u);
    EXPECT_EQ(v[0].val, 10);
    EXPECT_EQ(Tracked::alive, 1);
}

TEST(Vector, pop_back) {
    vector<int> v = {1, 2, 3};
    v.pop_back();
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v.back(), 2);
}

TEST(Vector, insert_single) {
    vector<int> v = {1, 2, 4};
    auto it = v.insert(v.begin() + 2, 3);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 4);
}

TEST(Vector, insert_at_end) {
    vector<int> v = {1, 2};
    v.insert(v.end(), 3);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v.back(), 3);
}

TEST(Vector, erase_single) {
    vector<int> v = {1, 2, 3, 4};
    auto it = v.erase(v.begin() + 1);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 3);
    EXPECT_EQ(v[2], 4);
}

TEST(Vector, erase_range) {
    vector<int> v = {1, 2, 3, 4, 5};
    auto it = v.erase(v.begin() + 1, v.begin() + 3);
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 4);
    EXPECT_EQ(v[2], 5);
}

TEST(Vector, clear) {
    vector<int> v = {1, 2, 3};
    v.clear();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
}

TEST(Vector, resize_grow) {
    vector<int> v = {1, 2};
    v.resize(5, 99);
    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[2], 99);
    EXPECT_EQ(v[4], 99);
}

TEST(Vector, resize_shrink) {
    vector<int> v = {1, 2, 3, 4, 5};
    v.resize(3);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[2], 3);
}

// ============================================================================
// Assign
// ============================================================================

TEST(Vector, assign_range) {
    vector<int> v = {1, 2, 3};
    int arr[] = {10, 20};
    v.assign(arr, arr + 2);
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
}

TEST(Vector, assign_count) {
    vector<int> v;
    v.assign(3, 7);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 7);
}

// ============================================================================
// Iterators
// ============================================================================

TEST(Vector, iterators) {
    vector<int> v = {1, 2, 3};
    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6);
}

TEST(Vector, reverse_iterators) {
    vector<int> v = {1, 2, 3};
    auto rit = v.rbegin();
    EXPECT_EQ(*rit, 3);
    ++rit;
    EXPECT_EQ(*rit, 2);
    ++rit;
    EXPECT_EQ(*rit, 1);
    ++rit;
    EXPECT_TRUE(rit == v.rend());
}

TEST(Vector, const_iterators) {
    const vector<int> v = {10, 20};
    EXPECT_EQ(*v.cbegin(), 10);
    EXPECT_EQ(*(v.cend() - 1), 20);
}

// ============================================================================
// Comparisons
// ============================================================================

TEST(Vector, equality) {
    vector<int> a = {1, 2, 3};
    vector<int> b = {1, 2, 3};
    vector<int> c = {1, 2, 4};
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

TEST(Vector, less_than) {
    vector<int> a = {1, 2, 3};
    vector<int> b = {1, 2, 4};
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

// ============================================================================
// Move semantics with reallocation
// ============================================================================

TEST(Vector, reallocate_moves) {
    Tracked::reset();
    {
        vector<Tracked> v;
        v.emplace_back(1);
        v.emplace_back(2);
        v.emplace_back(3);
        v.emplace_back(4);
        EXPECT_EQ(Tracked::alive, 4);
        int moves_before = Tracked::moves;

        v.emplace_back(5);                        // reallocation: 4→8
        EXPECT_GT(Tracked::moves, moves_before);  // additional moves occurred
        EXPECT_EQ(Tracked::alive, 5);
    }
    EXPECT_EQ(Tracked::alive, 0);
}

// ============================================================================
// Swap
// ============================================================================

TEST(Vector, swap) {
    vector<int> a = {1, 2, 3};
    vector<int> b = {4, 5};
    a.swap(b);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(b[0], 1);
}

TEST(Vector, swap_empty) {
    vector<int> a = {1, 2};
    vector<int> b;
    a.swap(b);
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(b.size(), 2u);
}
