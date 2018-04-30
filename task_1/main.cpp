#include <iostream>
#include <ctime>

#include "StackAllocator.h"
#include "XorList.h"

double stopwatch() {
    static clock_t last;
    clock_t now = clock();
    double elapsedTime = static_cast<double>(now - last) / CLOCKS_PER_SEC;
    last = now;
    return elapsedTime;
}

int main() {
    stopwatch();
    XorList<int> x1(1'000'000, 5);
    std::cout << stopwatch() << std::endl;
    
    stopwatch();
    XorList<int, StackAllocator<int>> x2(1'000'000, 5);
    std::cout << stopwatch() << std::endl;
    
    return 0;
}
