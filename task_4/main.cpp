#include "ahalaimahalai.cpp"
#include <iostream>

int main() {
    constexpr int who = AhalaiMahalai<1, 1, 1>::who;
    constexpr int whence = AhalaiMahalai<1, 1, 1>::whence;
    constexpr int how = AhalaiMahalai<1, 1, 1>::how;
    std::cout << who << ' ' << whence << ' ' << how;
}
