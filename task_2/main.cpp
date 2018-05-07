/*
 * sample usage of allocation schema
 */

// #define _print_deallocate

#include "MemoryManagement.cpp"
#include <iostream>


class StackAllocatorMini : public IMemoryManager {
    static const size_t _poolSize = 1e5;
    static std::unique_ptr<char[]> _pool;
    static std::unique_ptr<int[]> _x;
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
std::unique_ptr<char[]> StackAllocatorMini::_pool(new char[StackAllocatorMini::_poolSize]);
std::unique_ptr<int[]> StackAllocatorMini::_x(new int[StackAllocatorMini::_poolSize]);
size_t StackAllocatorMini::_taken = 0;


struct X : public CAllocatedOn<CurrentMemoryManager> {
    int a;
};

struct Y : public CAllocatedOn<RuntimeHeap> {
    int a;
};


int main() {
    CMemoryManagerSwitcher switcher1(new StackAllocatorMini());
    auto a = new X();
    auto a1 = new X();
    auto b = new Y();
    CMemoryManagerSwitcher switcher2(new DefaultAllocator());
    auto c = new X();
    delete a;
    delete a1;
    delete b;
    delete c;

    return 0;
}
