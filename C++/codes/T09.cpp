#include <iostream>
#include <numbers>

int main(){
    auto phi_single = std::numbers::phi_v<float>;
    auto phi_double = std::numbers::phi_v<double>;
    std::cout << "Type of phi_single: " << typeid(phi_single).name() << std::endl;
    std::cout << "Type of phi_double: " << typeid(phi_double).name() << std::endl;
}