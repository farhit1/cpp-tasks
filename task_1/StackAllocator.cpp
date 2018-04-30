#include "StackAllocator.h"

const MemoryPool::size_type MemoryPool::_blockSize = 2e8;

std::unique_ptr<char[]>
        MemoryPool::_block(new char[MemoryPool::_blockSize]);

size_t MemoryPool::_pos = 0;

template <typename T>
StackAllocator<T>::StackAllocator() {}

template <typename T>
StackAllocator<T>::~StackAllocator() {}

template <typename T>
typename StackAllocator<T>::pointer StackAllocator<T>::allocate(
        typename StackAllocator<int>::size_type n,
        typename StackAllocator<void>::const_pointer hint) {
    pointer start = reinterpret_cast<pointer>(
            &MemoryPool::_block[MemoryPool::_pos]
    );
    MemoryPool::_pos += n * sizeof(value_type);
    return start;
}

template<typename T>
void StackAllocator<T>::deallocate(T* p, unsigned long n) {}

template<typename T>
template<class U, class... Args>
void StackAllocator<T>::construct(U *p, Args&&... args) {
    new((void*)p) U(std::forward<Args>(args)...);
}

template<typename T>
template<class U>
void StackAllocator<T>::destroy(U *p) {
    ((T*)p)->~T();
}
