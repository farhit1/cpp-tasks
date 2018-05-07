#ifndef CPPS_IMEMORYMANAGER_H
#define CPPS_IMEMORYMANAGER_H

#include <cstdlib>
#include <memory>
#include <assert.h>

#ifdef _print_deallocate
#include <iostream>
#endif

class IMemoryManager {
    size_t _counter;

    virtual void* _Alloc(size_t size) = 0;
    virtual void _Free(void* ptr) = 0;

public:
    void* Alloc(size_t size) {
        ++_counter;
        return _Alloc(size);
    }

    void Free(void* ptr) {
        --_counter;
        _Free(ptr);
    }


    IMemoryManager() :
        _counter(0) {}

    ~IMemoryManager() {
        assert(_counter == 0);
    }
};


class DefaultAllocator : public IMemoryManager {
    void* _Alloc(size_t size) override {
        return malloc(size);
    }

    void _Free(void* ptr) override {
        return free(ptr);
    }
};


template<typename Strategy>
class CAllocatedOn {
    static const size_t additionalMemory = sizeof(std::shared_ptr<IMemoryManager>);

protected:
    static void* Alloc(size_t n) {
        auto allocator = Strategy::getAllocator();
        void* p = allocator->Alloc(n + additionalMemory);
        auto allocPlacement = reinterpret_cast<IMemoryManager**>(p);
        *allocPlacement = allocator;
        return reinterpret_cast<void*>(reinterpret_cast<intptr_t>(p) + additionalMemory);
    }

    static void Free(void* p) {
        p = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(p) - additionalMemory);
        auto allocator = reinterpret_cast<IMemoryManager**>(p);
#ifdef _print_deallocate
        std::cerr << typeid(*deallocator).name() << std::endl;
#endif
        (*allocator)->Free(p);
    }

public:
    static void* operator new(size_t n) {
        return Alloc(n);
    }
    static void* operator new[](size_t n) {
        return Alloc(n);
    }

    static void operator delete(void* p) {
        Free(p);
    }
    static void operator delete[](void* p) {
        Free(p);
    }
};


class RuntimeHeap {
    static IMemoryManager* _allocator;

public:
    static IMemoryManager* getAllocator() {
        return _allocator;
    }
};
IMemoryManager* RuntimeHeap::_allocator(new DefaultAllocator());


class CMemoryManagerSwitcher {
    friend class CurrentMemoryManager;

    static IMemoryManager* _active;
    const IMemoryManager* _previous;

public:
    explicit CMemoryManagerSwitcher(IMemoryManager* newAllocator) :
            _previous(_active) {
        _active = newAllocator;
    }

    ~CMemoryManagerSwitcher() {
        _active = const_cast<IMemoryManager*>(_previous);
    }
};
IMemoryManager* CMemoryManagerSwitcher::_active(new DefaultAllocator());


class CurrentMemoryManager {
public:
    static IMemoryManager* getAllocator() {
        return CMemoryManagerSwitcher::_active;
    }
};

#endif //CPPS_IMEMORYMANAGER_H
