/*
 * sample usage of allocation schema
 */

// #define _print_deallocate

#include "MemoryManagement.cpp"
#include <iostream>


class StackAllocatorMini : public IMemoryManager {
    static const size_t _poolSize = 1e7;
    static std::unique_ptr<char[]> _pool;
    static size_t _taken;

public:
    StackAllocatorMini() = default;
    ~StackAllocatorMini() = default;

    void* Alloc(size_t size) override {
        _taken += size;
        return reinterpret_cast<void*>(&_pool[_taken - size]);
    }

    void Free(void* p) override {}
};
std::unique_ptr<char[]> StackAllocatorMini::_pool(new char[StackAllocatorMini::_poolSize]);
size_t StackAllocatorMini::_taken = 0;


struct X : public CAllocatedOn<CurrentMemoryManager> {
    int a;
};

struct Y : public CAllocatedOn<RuntimeHeap> {
    int a;
};


int main() {
    CMemoryManagerSwitcher::switchAllocator(new StackAllocatorMini());
    auto a = new X();
    auto b = new Y();
    CMemoryManagerSwitcher::switchAllocator(new DefaultAllocator());
    auto c = new X();
    delete a;
    delete b;
    delete c;

    return 0;
}
