#include <iostream>

enum color {red, green, blue};
enum Day {Sun, Mon, Tue, Wed, Thu, Fri, Sat} n;
int main() {
    color c = blue;
    enum color c2 = green;
    n = Mon;
    enum Day n2 = Sun;
    std::cout << "The value of c is: " << c << std::endl;
    std::cout << "The value of c2 is: " << c2 << std::endl;
    std::cout << "The value of n is: " << n << std::endl;
    std::cout << "The value of n2 is: " << n2 << std::endl;
    return 0;
}