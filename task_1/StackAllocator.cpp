#include "StackAllocator.h"

std::unique_ptr<char[]>
        MemoryPool::block(new char[MemoryPool::blockSize]);

size_t MemoryPool::pos = 0;

std::unique_ptr<typename MemoryPool::BlockNode> MemoryPool::prevBlock = nullptr;

void* MemoryPool::blockGet(size_type n) {
    pos += n;
    return &block[pos - n];
}

void* MemoryPool::get(size_type n) {
    if (blockSize - pos < n) {
        auto x = std::unique_ptr<BlockNode>(new BlockNode{
                std::move(block),
                std::move(prevBlock)
        });
        prevBlock = std::move(x);
        block = std::move(std::unique_ptr<char[]>(new char[MemoryPool::blockSize]));
        pos = 0;
    }
    return blockGet(n);
}

template <typename T>
StackAllocator<T>::StackAllocator() {}

template <typename T>
StackAllocator<T>::~StackAllocator() {}

template <typename T>
typename StackAllocator<T>::pointer StackAllocator<T>::allocate(
        typename StackAllocator<int>::size_type n,
        typename StackAllocator<void>::const_pointer hint) {
    return reinterpret_cast<T*>(MemoryPool::get(n * sizeof(T)));
}

template<typename T>
void StackAllocator<T>::deallocate(T* p, unsigned long n) {}

template<typename T>
template<class U, class... Args>
void StackAllocator<T>::construct(U *p, Args&&... args) {
    new((void*)p) U(std::forward<Args>(args)...);
}

template<typename T>
void StackAllocator<T>::destroy(U *p) {
    p->~U();
}
