#ifndef CPPS_IMEMORYMANAGER_H
#define CPPS_IMEMORYMANAGER_H

#include <cstdlib>
#include <memory>
#include <cassert>

class IMemoryManager {
    size_t _counter;

    virtual void* _Alloc(size_t size) = 0;
    virtual void _Free(void* ptr) = 0;

public:
    void* Alloc(size_t size);

    void Free(void* ptr);

    IMemoryManager();

    ~IMemoryManager();
};


class DefaultAllocator : public IMemoryManager {
    void* _Alloc(size_t size) override;

    void _Free(void* ptr) override;
};


template<typename Strategy>
class CAllocatedOn {
    static const size_t additionalMemory = sizeof(std::shared_ptr<IMemoryManager>);

protected:
    static void* Alloc(size_t n);

    static void Free(void* p);

public:
    static void* operator new(size_t n);

    static void* operator new[](size_t n);

    static void operator delete(void* p);

    static void operator delete[](void* p);
};


class RuntimeHeap {
    static IMemoryManager* _allocator;

public:
    static IMemoryManager* getAllocator();
};


class CMemoryManagerSwitcher {
    friend class CurrentMemoryManager;

    static IMemoryManager* _active;
    IMemoryManager* _previous;

public:
    explicit CMemoryManagerSwitcher(IMemoryManager* newAllocator);

    ~CMemoryManagerSwitcher();
};
//extern IMemoryManager* CMemoryManagerSwitcher::_active;


class CurrentMemoryManager {
public:
    static IMemoryManager* getAllocator();
};


void* operator new(size_t n);

void* operator new[](size_t n);

void operator delete(void* p) noexcept;

void operator delete[](void* p) noexcept;

void checkDefaultAllocator();


///////////////////////////////////////////
// CAllocatedOn<Strategy> implementation //
///////////////////////////////////////////

template<typename Strategy>
void* CAllocatedOn<Strategy>::Alloc(size_t n) {
    auto allocator = Strategy::getAllocator();
    void* p = allocator->Alloc(n + additionalMemory);
    auto allocPlacement = reinterpret_cast<IMemoryManager**>(p);
    *allocPlacement = allocator;
    return reinterpret_cast<void*>(reinterpret_cast<intptr_t>(p) + additionalMemory);
}

template<typename Strategy>
void CAllocatedOn<Strategy>::Free(void* p) {
    p = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(p) - additionalMemory);
    auto allocator = reinterpret_cast<IMemoryManager**>(p);
    (*allocator)->Free(p);
}

template<typename Strategy>
void* CAllocatedOn<Strategy>::operator new(size_t n) {
    return Alloc(n);
}

template<typename Strategy>
void* CAllocatedOn<Strategy>::operator new[](size_t n) {
    return Alloc(n);
}

template<typename Strategy>
void CAllocatedOn<Strategy>::operator delete(void* p) {
    Free(p);
}

template<typename Strategy>
void CAllocatedOn<Strategy>::operator delete[](void* p) {
    Free(p);
}

#endif //CPPS_IMEMORYMANAGER_H
