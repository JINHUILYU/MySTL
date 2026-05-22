#include <iostream>
#include <cstring>
#include <cassert>

namespace demo
{
    class string
    {
    public:
        // 默认构造函数
        string(const char* str = "") {
            _size = strlen(str);            // 计算字符串长度
            _capacity = _size;              // 设置容量为字符串长度
            _str = new char[_capacity + 1]; // 动态分配内存
            strcpy(_str, str);              // 复制字符串内容
        }
        
        // 如果没有拷贝构造函数，编译器会生成一个默认的拷贝构造函数，这个默认的拷贝构造函数会进行浅拷贝，即直接复制指针地址。这会导致多个 string 对象共享同一块内存，当其中一个对象被销毁时，其他对象的指针将变为悬空指针，访问它们会导致未定义行为。
        // 拷贝构造函数（深拷贝）
        string(const string& other) {
            _size = other._size;
            _capacity = other._capacity;
            _str = new char[_capacity + 1]; // 重新分配独立内存
            strcpy(_str, other._str);       // 复制字符串内容
        }

        // 赋值运算符（深拷贝）
        string& operator=(const string& other) {
            if (this != &other) {    // 防止自赋值
                delete[] _str;       // 先释放当前对象的内存
                // 重新分配内存并复制内容
                _size = other._size;
                _capacity = other._capacity;
                _str = new char[_capacity + 1];
                strcpy(_str, other._str);
            }
            return *this;
        }

        // 析构函数
        ~string() {
            if (_str) {
                delete[] _str;  // 释放动态分配的内存
                _str = nullptr; // 避免悬空指针
            }
        }

        // 非 const 迭代器
        typedef char* iterator;
        iterator begin() { return _str; }
        iterator end() { return _str + _size; }

        // const 迭代器
        typedef const char* const_iterator;
        const_iterator begin() const { return _str; }
        const_iterator end() const { return _str + _size; }

        // 获取 C 风格字符串
        const char* c_str() const {
            return _str;
        }

        // 查找字符在字符串中从指定位置开始，第一次出现的位置
        size_t find(char ch, size_t pos = 0) const {
            assert(pos <= _size); // 确保起始位置合法
            for (size_t i = pos; i < _size; ++i) {
                if (_str[i] == ch) {
                    return i; // 返回字符位置
                }
            }
            return npos; // 未找到返回 npos
        }

        // 查找子字符串在字符串中从指定位置开始，第一次出现的位置
        size_t find(const char* substr, size_t pos = 0) const {
            assert(pos <= _size); // 确保起始位置合法
            size_t substr_len = strlen(substr);
            if (substr_len == 0) {
                return pos <= _size ? pos : npos; // 空字符串特殊处理
            }
            for (size_t i = pos; i <= _size - substr_len; ++i) {
                if (strncmp(_str + i, substr, substr_len) == 0) {
                    return i; // 返回子字符串位置
                }
            }
            return npos; // 未找到返回 npos
        }

        // 在指定位置插入字符
        string& insert(size_t pos, char ch) {
            assert(pos <= _size); // 确保插入位置合法
            if (_size + 1 > _capacity) {
                // 需要扩容
                _capacity = (_capacity == 0) ? 1 : _capacity * 2; // 扩容策略
                char* new_str = new char[_capacity + 1];
                strncpy(new_str, _str, pos); // 复制插入位置之前的内容
                new_str[pos] = ch;           // 插入新字符
                strncpy(new_str + pos + 1, _str + pos, _size - pos); // 复制插入位置之后的内容
                new_str[_size + 1] = '\0';   // 添加字符串结束符
                delete[] _str;               // 释放旧内存
                _str = new_str;              // 更新指针
            } else {
                // 不需要扩容，直接移动后续字符并插入新字符
                memmove(_str + pos + 1, _str + pos, _size - pos + 1); // 包括结束符一起移动
                _str[pos] = ch; // 插入新字符
            }
            ++_size; // 更新大小
            return *this;
        }

        // 在指定位置插入子字符串
        string& insert(size_t pos, const char* substr) {
            assert(pos <= _size); // 确保插入位置合法
            size_t substr_len = strlen(substr);
            if (_size + substr_len > _capacity) {
                // 需要扩容
                _capacity = (_capacity == 0) ? substr_len : std::max(_capacity * 2, _size + substr_len); // 扩容策略
                char* new_str = new char[_capacity + 1];
                strncpy(new_str, _str, pos); // 复制插入位置之前的内容
                strncpy(new_str + pos, substr, substr_len); // 插入新子字符串
                strncpy(new_str + pos + substr_len, _str + pos, _size - pos); // 复制插入位置之后的内容
                new_str[_size + substr_len] = '\0'; // 添加字符串结束符
                delete[] _str; // 释放旧内存
                _str = new_str; // 更新指针
            } else {
                // 不需要扩容，直接移动后续字符并插入新子字符串
                memmove(_str + pos + substr_len, _str + pos, _size - pos + 1); // 包括结束符一起移动
                memcpy(_str + pos, substr, substr_len); // 插入新子字符串
            }
            _size += substr_len; // 更新大小
            return *this;
        }

        // 在指定位置删除若干字符
        string& erase(size_t pos, size_t count = 1) {
            assert(pos <= _size); // 确保删除位置合法
            if (pos + count > _size) {
                count = _size - pos; // 调整删除数量
            }
            memmove(_str + pos, _str + pos + count, _size - pos - count + 1); // 包括结束符一起移动
            _size -= count; // 更新大小
            return *this;
        }

    static const size_t npos = -1; // 表示未找到
    
    private:
        char* _str;       // 存储字符串的字符数组
        size_t _capacity; // 分配的内存容量
        size_t _size;     // 当前字符串的有效长度
    };
}

void TestString()
{
    std::cout << "--- Testing string start. ---" << std::endl;

    demo::string s1("Hello, World!");
    std::cout << "s1: " << s1.c_str() << std::endl;

    demo::string s2 = s1; // 使用拷贝构造函数
    std::cout << "s2 (copy of s1): " << s2.c_str() << std::endl;

    demo::string s3;
    s3 = s1; // 使用赋值运算符
    std::cout << "s3 (assigned from s1): " << s3.c_str() << std::endl;

    std::cout << "--- Testing string end. ---" << std::endl;
}

void TestIterator()
{
    std::cout << "--- Testing iterators start. ---" << std::endl;

    std::cout << "Iterating using non-const iterators: ";
    demo::string s("Hello, World!");
    for (demo::string::iterator it = s.begin(); it != s.end(); ++it) {
        std::cout << (char)toupper(*it);
        *it = toupper(*it); // 转换为大写并修改字符串内容
    }
    std::cout << std::endl;
    std::cout << "Modified string: " << s.c_str() << std::endl;

    std::cout << "Iterating using const iterators: ";
    demo::string s_const("Hello, World!");
    for (demo::string::const_iterator it = s_const.begin(); it != s_const.end(); ++it) {
        std::cout << *it; // 这里使用 const 迭代器，不能修改字符串内容
    }
    std::cout << std::endl;

    for (auto& ch : s) {
        ch = tolower(ch); // 转换为小写
    }
    std::cout << s.c_str() << std::endl; // 输出：hello world!

    // 范围for循环遍历const对象
    for (const auto& ch : s_const) {
        std::cout << ch; // 只能读取，不能修改
    }
    std::cout << std::endl;

    std::cout << "--- Testing iterators end. ---" << std::endl;
}

void TestFind()
{
    std::cout << "--- Testing find start. ---" << std::endl;

    demo::string s("Hello, World!");
    size_t pos = s.find('o');
    if (pos != demo::string::npos) {
        std::cout << "Character 'o' found at position: " << pos << std::endl;
    } else {
        std::cout << "Character 'o' not found." << std::endl;
    }

    pos = s.find("World");
    if (pos != demo::string::npos) {
        std::cout << "Substring 'World' found at position: " << pos << std::endl;
    } else {
        std::cout << "Substring 'World' not found." << std::endl;
    }

    pos = s.find("C++");
    if (pos != demo::string::npos) {
        std::cout << "Substring 'C++' found at position: " << pos << std::endl;
    } else {
        std::cout << "Substring 'C++' not found." << std::endl;
    }

    std::cout << "--- Testing find end. ---" << std::endl;
}

void TestInsert()
{
    std::cout << "--- Testing insert start. ---" << std::endl;

    demo::string s("Hello, World!");
    s.insert(5, ','); // 在位置 5 插入字符 ','
    std::cout << "After inserting ',': " << s.c_str() << std::endl;

    s.insert(6, " MySTL"); // 在位置 6 插入子字符串 " MySTL"
    std::cout << "After inserting ' MySTL': " << s.c_str() << std::endl;

    std::cout << "--- Testing insert end. ---" << std::endl;
}

void TestErase()
{
    std::cout << "--- Testing erase start. ---" << std::endl;

    demo::string s("Hello, World!");
    s.erase(5, 1); // 删除位置 5 的字符 ','
    std::cout << "After erasing ',': " << s.c_str() << std::endl;

    s.erase(5, 6); // 删除位置 5 开始的 6 个字符 " MySTL"
    std::cout << "After erasing ' MySTL': " << s.c_str() << std::endl;

    std::cout << "--- Testing erase end. ---" << std::endl;
}

int main() {
    demo::string s("Hello, World!");
    std::cout << s.c_str() << std::endl;

    demo::string s1("Hello C++");
    demo::string s2(s1); // 深拷贝，s1 和 s2 各自拥有独立的内存
    std::cout << s1.c_str() << std::endl;
    std::cout << s2.c_str() << std::endl;
    
    demo::string s3("Hello C++");
    demo::string s4 = s3; // 赋值运算符，深拷贝，s3 和 s4 各自拥有独立的内存
    std::cout << s3.c_str() << std::endl;
    std::cout << s4.c_str() << std::endl;

    TestString();
    TestIterator();
    TestFind();
    TestInsert();
    TestErase();
    
    return 0;
}
