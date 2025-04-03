#include <iostream>

int main() {
    const int i = 10;
    int& e = const_cast<int&>(i); // 将const int类型转换为int类型
    e = 20; // 成功修改i的值
    std::cout << "i = " << i << std::endl; // 输出i的值 
    std::cout << "e = " << e << std::endl; // 输出e的值
}   