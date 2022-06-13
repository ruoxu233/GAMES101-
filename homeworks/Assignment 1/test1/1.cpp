#include<iostream>

namespace color 
{
    enum Type
    {
        cout=15,
        YELLOW,
        BLUE
    };

    int a = 100;
};
    
int main(int argc, const char** argv){
    std::cout << "Example of cpp\n";
    std::cout << color::BLUE << ' ' << color::YELLOW << '\n';
    std::cout << color::a * 2 << '\n';
    std::cout << argc << '\n';
    std::cout << argv << '\n';
    std::cout << argv[1] << '\n';
    std::cout << argv[2] << '\n';
    return 0;
}