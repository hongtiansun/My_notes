#include <iostream>
using namespace std;
int main()
{
    int index = 10;
    int num =100;
    auto function = ([=]{cout<<"num="<<num<<",index="<<index<<endl;});
    function();
    cout<<"num="<<num<<",index="<<index<<endl;
}