#include <iostream>
#include <ctime>

#include "StackAllocator.h"
#include "XorList.h"

int main() {
    int a = clock();
    XorList<int> x1(1'000'000, 5);
    std::cout << double(clock() - a) / CLOCKS_PER_SEC << std::endl;

    int b = clock();
    XorList<int, StackAllocator<int>> x2(1'000'000, 5);
    std::cout << double(clock() - b) / CLOCKS_PER_SEC << std::endl;
    
    return 0;
}
