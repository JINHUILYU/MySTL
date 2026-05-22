#include <gtest/gtest.h>
#include <mystl/string.h>

using namespace mystl;

// ============================================================================
// Construction
// ============================================================================

TEST(String, default_construct) {
    string s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
    EXPECT_EQ(s.length(), 0u);
    EXPECT_STREQ(s.c_str(), "");
}

TEST(String, cstr_construct) {
    string s("hello");
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "hello");
}

TEST(String, fill_construct) {
    string s(5, 'a');
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "aaaaa");
}

TEST(String, copy_construct) {
    string s1("hello");
    string s2(s1);
    EXPECT_EQ(s2.size(), 5u);
    EXPECT_STREQ(s2.c_str(), "hello");
    // Deep copy: s1 unaffected by modifying s2
    s2[0] = 'H';
    EXPECT_EQ(s1[0], 'h');
}

TEST(String, move_construct) {
    string s1("hello");
    string s2(mystl::move(s1));
    EXPECT_EQ(s2.size(), 5u);
    EXPECT_TRUE(s1.empty());
    EXPECT_STREQ(s1.c_str(), "");
}

TEST(String, initializer_list) {
    string s = {'a', 'b', 'c'};
    EXPECT_EQ(s.size(), 3u);
    EXPECT_STREQ(s.c_str(), "abc");
}

TEST(String, substr_construct) {
    string s("hello world");
    string sub(s, 6, 5);
    EXPECT_STREQ(sub.c_str(), "world");
}

// ============================================================================
// Element access
// ============================================================================

TEST(String, element_access) {
    string s = "abc";
    EXPECT_EQ(s.front(), 'a');
    EXPECT_EQ(s.back(), 'c');
    EXPECT_EQ(s[1], 'b');
    EXPECT_EQ(s.at(0), 'a');
    EXPECT_THROW(s.at(5), const char*);
    EXPECT_STREQ(s.data(), "abc");
    EXPECT_STREQ(s.c_str(), "abc");
}

TEST(String, mutable_access) {
    string s = "abc";
    s[1] = 'x';
    EXPECT_STREQ(s.c_str(), "axc");
}

TEST(String, null_terminated) {
    string s = "hi";
    EXPECT_EQ(s.c_str()[2], '\0');
    EXPECT_EQ(s.data()[2], '\0');
}

// ============================================================================
// Capacity
// ============================================================================

TEST(String, reserve) {
    string s;
    s.reserve(100);
    EXPECT_GE(s.capacity(), 100u);
    EXPECT_EQ(s.size(), 0u);
    EXPECT_NE(s.data(), nullptr);
}

TEST(String, shrink_to_fit) {
    string s;
    s.reserve(100);
    s += "ab";
    size_t old_cap = s.capacity();
    EXPECT_GE(old_cap, 100u);
    s.shrink_to_fit();
    EXPECT_LE(s.capacity(), old_cap);
    EXPECT_STREQ(s.c_str(), "ab");
}

TEST(String, empty_check) {
    string s;
    EXPECT_TRUE(s.empty());
    s += 'x';
    EXPECT_FALSE(s.empty());
    s.clear();
    EXPECT_TRUE(s.empty());
}

// ============================================================================
// Modifiers
// ============================================================================

TEST(String, push_back) {
    string s;
    s.push_back('a');
    s.push_back('b');
    s.push_back('c');
    EXPECT_EQ(s.size(), 3u);
    EXPECT_STREQ(s.c_str(), "abc");
}

TEST(String, pop_back) {
    string s = "abc";
    s.pop_back();
    EXPECT_EQ(s.size(), 2u);
    EXPECT_STREQ(s.c_str(), "ab");
}

TEST(String, append_cstr) {
    string s = "hello";
    s.append(" world");
    EXPECT_STREQ(s.c_str(), "hello world");
}

TEST(String, append_count) {
    string s = "a";
    s.append(3, 'b');
    EXPECT_STREQ(s.c_str(), "abbb");
}

TEST(String, append_string) {
    string s1 = "hello";
    string s2 = " world";
    s1.append(s2);
    EXPECT_STREQ(s1.c_str(), "hello world");
}

TEST(String, operator_plus_eq) {
    string s = "hello";
    s += " world";
    s += '!';
    EXPECT_STREQ(s.c_str(), "hello world!");
}

TEST(String, clear) {
    string s = "hello";
    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_STREQ(s.c_str(), "");
}

TEST(String, insert_char) {
    string s = "ac";
    s.insert(s.begin() + 1, 'b');
    EXPECT_STREQ(s.c_str(), "abc");
}

TEST(String, insert_count) {
    string s = "ad";
    s.insert(1, 2, 'x');
    EXPECT_STREQ(s.c_str(), "axxd");
}

TEST(String, insert_cstr) {
    string s = "ad";
    string ins = "bc";
    s.insert(1, ins);
    EXPECT_STREQ(s.c_str(), "abcd");
}

TEST(String, insert_at_end) {
    string s = "ab";
    s.insert(s.end(), 'c');
    EXPECT_STREQ(s.c_str(), "abc");
}

TEST(String, erase_single) {
    string s = "abc";
    s.erase(s.begin() + 1);
    EXPECT_STREQ(s.c_str(), "ac");
}

TEST(String, erase_range) {
    string s = "aXXXbc";
    s.erase(s.begin() + 1, s.begin() + 4);
    EXPECT_STREQ(s.c_str(), "abc");
}

TEST(String, resize_grow) {
    string s = "ab";
    s.resize(5, 'x');
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "abxxx");
}

TEST(String, resize_shrink) {
    string s = "abcde";
    s.resize(3);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_STREQ(s.c_str(), "abc");
}

// ============================================================================
// Assignment
// ============================================================================

TEST(String, assign_copy) {
    string s1 = "hello";
    string s2 = "world";
    s1 = s2;
    EXPECT_STREQ(s1.c_str(), "world");
}

TEST(String, assign_move) {
    string s1 = "hello";
    string s2 = "world";
    s1 = mystl::move(s2);
    EXPECT_STREQ(s1.c_str(), "world");
    EXPECT_TRUE(s2.empty());
}

TEST(String, assign_cstr) {
    string s = "old";
    s = "new";
    EXPECT_STREQ(s.c_str(), "new");
}

TEST(String, assign_char) {
    string s = "old";
    s = 'x';
    EXPECT_STREQ(s.c_str(), "x");
}

// ============================================================================
// String operations
// ============================================================================

TEST(String, substr) {
    string s = "hello world";
    EXPECT_STREQ(s.substr(0, 5).c_str(), "hello");
    EXPECT_STREQ(s.substr(6).c_str(), "world");
    EXPECT_STREQ(s.substr(6, 2).c_str(), "wo");
}

TEST(String, find_cstr) {
    string s = "hello world hello";
    EXPECT_EQ(s.find("hello"), 0u);
    EXPECT_EQ(s.find("world"), 6u);
    EXPECT_EQ(s.find("hello", 1), 12u);
    EXPECT_EQ(s.find("xyz"), string::npos);
}

TEST(String, find_char) {
    string s = "hello";
    EXPECT_EQ(s.find('h'), 0u);
    EXPECT_EQ(s.find('l'), 2u);
    EXPECT_EQ(s.find('l', 3), 3u);
    EXPECT_EQ(s.find('x'), string::npos);
}

TEST(String, rfind) {
    string s = "hello world hello";
    EXPECT_EQ(s.rfind("hello"), 12u);
    EXPECT_EQ(s.rfind("hello", 10), 0u);
    EXPECT_EQ(s.rfind('l'), 15u);
    EXPECT_EQ(s.rfind('l', 5), 3u);
    EXPECT_EQ(s.rfind("xyz"), string::npos);
}

TEST(String, find_first_of) {
    string s = "hello";
    EXPECT_EQ(s.find_first_of("aeiou"), 1u);  // 'e' at pos 1
    EXPECT_EQ(s.find_first_of("xyz"), string::npos);
}

TEST(String, find_last_of) {
    string s = "hello";
    EXPECT_EQ(s.find_last_of("aeiou"), 4u);  // 'o' at pos 4
    EXPECT_EQ(s.find_last_of("xyz"), string::npos);
}

TEST(String, compare) {
    string a = "abc";
    string b = "abc";
    string c = "abd";
    string d = "ab";

    EXPECT_EQ(a.compare(b), 0);
    EXPECT_LT(a.compare(c), 0);
    EXPECT_GT(c.compare(a), 0);
    EXPECT_GT(a.compare(d), 0);
}

// ============================================================================
// Iterators
// ============================================================================

TEST(String, iterators) {
    string s = "abc";
    string result;
    for (auto it = s.begin(); it != s.end(); ++it) {
        result.push_back(*it);
    }
    EXPECT_STREQ(result.c_str(), "abc");
}

TEST(String, reverse_iterators) {
    string s = "abc";
    auto rit = s.rbegin();
    EXPECT_EQ(*rit, 'c');
    ++rit;
    EXPECT_EQ(*rit, 'b');
    ++rit;
    EXPECT_EQ(*rit, 'a');
    ++rit;
    EXPECT_TRUE(rit == s.rend());
}

TEST(String, const_iterators) {
    const string s = "ab";
    EXPECT_EQ(*s.cbegin(), 'a');
    EXPECT_EQ(*(s.cend() - 1), 'b');
}

// ============================================================================
// Comparisons
// ============================================================================

TEST(String, equality) {
    string a = "abc";
    string b = "abc";
    string c = "abd";
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

TEST(String, less_than) {
    string a = "abc";
    string b = "abd";
    string c = "ab";
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
    EXPECT_TRUE(c < a);
    EXPECT_TRUE(a >= c);
}

TEST(String, comparison_operators) {
    string a = "abc";
    string b = "abc";
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(a >= b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(a > b);
}

// ============================================================================
// Concatenation
// ============================================================================

TEST(String, operator_plus) {
    string a = "hello";
    string b = " world";
    string result = a + b;
    EXPECT_STREQ(result.c_str(), "hello world");
    EXPECT_STREQ(a.c_str(), "hello");  // originals unchanged
}

TEST(String, operator_plus_cstr) {
    string s = "hello";
    string result = s + "!";
    EXPECT_STREQ(result.c_str(), "hello!");

    string result2 = "!" + s;
    EXPECT_STREQ(result2.c_str(), "!hello");
}

TEST(String, operator_plus_char) {
    string s = "abc";
    string result = s + 'd';
    EXPECT_STREQ(result.c_str(), "abcd");

    string result2 = 'd' + s;
    EXPECT_STREQ(result2.c_str(), "dabc");
}

// ============================================================================
// Swap
// ============================================================================

TEST(String, swap) {
    string a = "hello";
    string b = "world";
    a.swap(b);
    EXPECT_STREQ(a.c_str(), "world");
    EXPECT_STREQ(b.c_str(), "hello");
}

TEST(String, swap_empty) {
    string a = "hello";
    string b;
    a.swap(b);
    EXPECT_TRUE(a.empty());
    EXPECT_STREQ(b.c_str(), "hello");
}

// ============================================================================
// Reallocation correctness
// ============================================================================

TEST(String, reallocation_preserves_content) {
    string s = "hello";
    s.reserve(4);  // no realloc, already >=
    EXPECT_STREQ(s.c_str(), "hello");

    s.reserve(100);
    EXPECT_STREQ(s.c_str(), "hello");  // content preserved after realloc
}

TEST(String, many_push_backs) {
    string s;
    for (int i = 0; i < 100; ++i) {
        s.push_back('a' + (i % 26));
    }
    EXPECT_EQ(s.size(), 100u);
    EXPECT_EQ(s.front(), 'a');
    EXPECT_EQ(s.back(), 'a' + (99 % 26));
    EXPECT_EQ(s.c_str()[100], '\0');  // null terminated
}
