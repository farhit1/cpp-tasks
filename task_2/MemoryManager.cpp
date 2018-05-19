#include <iostream>
#include "MemoryManager.h"

bool defaultAllocatorInit;
DefaultAllocator defaultAllocator;

void* IMemoryManager::Alloc(size_t size) {
    ++_counter;
    return _Alloc(size);
}

void IMemoryManager::Free(void* ptr) {
    --_counter;
    _Free(ptr);
}

IMemoryManager::IMemoryManager() :
        _counter(0) {}

IMemoryManager::~IMemoryManager() {
    assert(_counter == 0);
}

void* DefaultAllocator::_Alloc(size_t size) {
    return malloc(size);
}

void DefaultAllocator::_Free(void* ptr) {
    // std::cerr << "free on default allocator\n";
    return free(ptr);
}

void checkDefaultAllocator() {
    if (!defaultAllocatorInit) {
        defaultAllocatorInit = true;
        DefaultAllocator da;
        std::swap(defaultAllocator, da);
    }
};

IMemoryManager* RuntimeHeap::getAllocator() {
    checkDefaultAllocator();
    return &defaultAllocator;
}

IMemoryManager* RuntimeHeap::_allocator(nullptr);

CMemoryManagerSwitcher::CMemoryManagerSwitcher(IMemoryManager* newAllocator) :
        _previous(_active) {
    _active = newAllocator;
}

CMemoryManagerSwitcher::~CMemoryManagerSwitcher() {
    _active = _previous;
}
IMemoryManager* CMemoryManagerSwitcher::_active(nullptr);

IMemoryManager* CurrentMemoryManager::getAllocator() {
    if (!CMemoryManagerSwitcher::_active) {
        checkDefaultAllocator();
        CMemoryManagerSwitcher::_active = &defaultAllocator;
    }
    return CMemoryManagerSwitcher::_active;
}

void* operator new(size_t n) {
    return CAllocatedOn<RuntimeHeap>::operator new(n);
}

void* operator new[](size_t n) {
    return CAllocatedOn<RuntimeHeap>::operator new[](n);
}

void operator delete(void* p) noexcept {
    CAllocatedOn<RuntimeHeap>::operator delete(p);
}

void operator delete[](void* p) noexcept {
    CAllocatedOn<RuntimeHeap>::operator delete[](p);
}
