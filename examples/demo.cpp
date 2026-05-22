#include <iostream>

#include "mystl/vector.h"
#include "mystl/string.h"

int main() {
    mystl::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;

    mystl::string str = "Hello, MySTL!";
    std::cout << str.c_str() << std::endl;

    return 0;
}