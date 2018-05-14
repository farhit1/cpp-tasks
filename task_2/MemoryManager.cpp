#include "MemoryManager.h"

DefaultAllocator defaultAllocatorInstance;

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
    return free(ptr);
}

IMemoryManager* RuntimeHeap::getAllocator() {
    if (!_allocator)
        _allocator = &defaultAllocatorInstance;
    return _allocator;
}

IMemoryManager* RuntimeHeap::_allocator(nullptr);

CMemoryManagerSwitcher::CMemoryManagerSwitcher(IMemoryManager* newAllocator) :
        _previous(_active) {
    _active = newAllocator;
}

CMemoryManagerSwitcher::~CMemoryManagerSwitcher() {
    _active = const_cast<IMemoryManager*>(_previous);
}
IMemoryManager* CMemoryManagerSwitcher::_active(nullptr);

IMemoryManager* CurrentMemoryManager::getAllocator() {
    if (!CMemoryManagerSwitcher::_active)
        CMemoryManagerSwitcher::_active = &defaultAllocatorInstance;
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
