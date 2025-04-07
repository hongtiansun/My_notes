#include <iostream>

namespace {
    int x =10;
    void print(const char* msg){
        std::cout << msg << std::endl;
    }
}

int main(){
    std::cout << "x = " << x << std::endl;
    print("Hello, World!");
}