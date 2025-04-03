#include <iostream>

int main(){
    int i = 10;
    float f = static_cast<float>(i); // 静态将int类型转换为float类型
    std::cout << "i = " << i << std::endl;
    std::cout << "f = " << f << std::endl;
}