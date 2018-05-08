#include <iostream>
#include "Tuple.h"


int main() {
    const int i1 = 5;
    int&& i2 = 42;
    double d = 3.14;
    auto s = std::string("string");

    auto tuple_3int = Tuple<int, int, int>(i1, 100, i2);
    std::cout << get<0>(tuple_3int) << std::endl;  // 5
    std::cout << get<1>(tuple_3int) << std::endl;  // 100
    std::cout << get<2>(tuple_3int) << std::endl;  // 42
    get<0>(tuple_3int) = 22;
    std::cout << get<0>(tuple_3int) << std::endl;  // 22

    auto tuple_ds = makeTuple(d, s);
    std::cout << get<0>(tuple_ds) << std::endl;  // 3.14
    d = 3.15;
    std::cout << get<0>(tuple_ds) << std::endl;  // still 3.14
//    get<1>(const_cast<const decltype(tuple_ds)&>(tuple_ds)) = "435";  // not assignable, const &
//    get<0>(makeTuple(0)) = 0;  // not assignable, &&

    std::cout << get<int>(tuple_3int) << std::endl;  // works
//    std::cout << get<std::string>(tuple_3int) << std::endl;  // doesn't work

    auto f = tupleCat(tuple_3int, tuple_ds, makeTuple(), makeTuple(std::string("one more string")));
    std::cout << get<0>(f) << std::endl;  // 22
    std::cout << get<1>(f) << std::endl;  // 100
    std::cout << get<2>(f) << std::endl;  // 42
    std::cout << get<3>(f) << std::endl;  // 3.14
    std::cout << get<4>(f) << std::endl;  // string
    std::cout << get<5>(f) << std::endl;  // one more string
//    std::cout << get<6>(f) << std::endl;  // fail

    get<1>(tuple_ds) = "another string";
    std::cout << get<4>(f) << std::endl;  // still string

    auto a = makeTuple(1, 2);
    auto b = makeTuple(1, 2);
    std::cout << (a == b) << std::endl;  // true
    std::cout << (a < makeTuple(0, 2)) << std::endl;  // false

    return 0;
}
