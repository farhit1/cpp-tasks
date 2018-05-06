#ifndef CPPS_IMEMORYMANAGER_H
#define CPPS_IMEMORYMANAGER_H

#include <cstdlib>
#include <memory>

#ifdef _print_deallocate
#include <iostream>
#endif

class IMemoryManager {
public:
    virtual void* Alloc(size_t size) = 0;
    virtual void Free(void* ptr) = 0;
};


class DefaultAllocator : public IMemoryManager {
public:
    void* Alloc(size_t size) override {
        return malloc(size);
    }

    void Free(void* ptr) override {
        return free(ptr);
    }
};


template<typename Strategy>
class CAllocatedOn {
    static const size_t additionalMemory = sizeof(std::shared_ptr<IMemoryManager>);

protected:
    static void* Alloc(size_t n) {
        std::shared_ptr<IMemoryManager> allocator = Strategy::getAllocator();
        void* p = allocator->Alloc(n + additionalMemory);
        new(p) std::shared_ptr<IMemoryManager>(allocator);
        return reinterpret_cast<void*>(reinterpret_cast<intptr_t>(p) + additionalMemory);
    }

    static void Free(void* p) {
        p = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(p) - additionalMemory);
        auto deallocator = *reinterpret_cast<std::shared_ptr<IMemoryManager>*>(p);
#ifdef _print_deallocate
        std::cerr << typeid(*deallocator).name() << std::endl;
#endif
        deallocator->Free(p);
        reinterpret_cast<std::shared_ptr<IMemoryManager>*>(p)->reset();
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
    static std::shared_ptr<IMemoryManager> _allocator;

public:
    static std::shared_ptr<IMemoryManager> getAllocator() {
        return _allocator;
    }
};
std::shared_ptr<IMemoryManager> RuntimeHeap::_allocator(new DefaultAllocator());


class CMemoryManagerSwitcher {
    friend class CurrentMemoryManager;

    static std::shared_ptr<IMemoryManager> _active;
public:
    static void switchAllocator(IMemoryManager* newAllocator) {
        _active.reset(newAllocator);
    }
};
std::shared_ptr<IMemoryManager> CMemoryManagerSwitcher::_active(new DefaultAllocator());


class CurrentMemoryManager {
public:
    static std::shared_ptr<IMemoryManager> getAllocator() {
        return CMemoryManagerSwitcher::_active;
    }
};

#endif //CPPS_IMEMORYMANAGER_H
