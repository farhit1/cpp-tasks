/*
 * sample usage of allocation schema
 */

#include "MemoryManager.h"
#include <iostream>

/*
struct A {
    int *a;
    A() {
        a = new int(5);
    }
};

static A a();
*/

class StackAllocatorMini : public IMemoryManager {
    static const size_t _poolSize = 1e5;
    static char _pool[_poolSize];
    static char _x[_poolSize];
    static size_t _taken;

    void* _Alloc(size_t size) override {
        _taken += size;
        _x[_taken - size] = size;
        return reinterpret_cast<void*>(&_pool[_taken - size]);
    }

    void _Free(void* p) override {
        int size = _x[reinterpret_cast<char*>(p) - &_pool[0]];
        for (size_t i = 0; i < size; ++i)
            *(reinterpret_cast<char*>(p) + i) = 0xff;
    }
};
char StackAllocatorMini::_pool[] = {};
char StackAllocatorMini::_x[] = {};
size_t StackAllocatorMini::_taken = 0;


struct X : public CAllocatedOn<CurrentMemoryManager> {
    int a;
    X(int a) : a(a) {}
};

struct Y : public CAllocatedOn<RuntimeHeap> {
    int a;
    Y(int a) : a(a) {}
};


int main() {
    StackAllocatorMini alloc1;
    CMemoryManagerSwitcher switcher1(&alloc1);

    auto a = new X(5);
    auto a1 = new X(6);
    auto b = new Y(7);

    std::cout << a->a << " " << a1->a << std::endl;
    std::cout << b->a << std::endl;

    delete a;
    delete a1;

    DefaultAllocator alloc2;
    CMemoryManagerSwitcher switcher2(&alloc2);
    auto c = new X(8);
    delete b;
    delete c;

    return 0;
}
