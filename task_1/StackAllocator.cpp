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
    if (n > blockSize)
        throw std::bad_alloc();
    if (blockSize - pos < n) {
        auto x = std::make_unique<BlockNode>(
                std::move(block),
                std::move(prevBlock)
        );
        prevBlock = std::move(x);
        block.reset(new char[MemoryPool::blockSize]);
        pos = 0;
    }
    return blockGet(n);
}

MemoryPool::BlockNode::BlockNode(std::unique_ptr<char[]> block,
                                 std::unique_ptr<BlockNode> nextBlock) :
        block(std::move(block)),
        nextBlock(std::move(nextBlock)) {}

template <typename T>
StackAllocator<T>::StackAllocator() {}

template <typename T>
template <class U>
StackAllocator<T>::StackAllocator(const StackAllocator<U>& other) {}

template <typename T>
StackAllocator<T>::~StackAllocator() {}

template <typename T>
typename StackAllocator<T>::pointer StackAllocator<T>::allocate(
        typename StackAllocator<int>::size_type n) {
    return reinterpret_cast<T*>(MemoryPool::get(n * sizeof(T)));
}

template<typename T>
void StackAllocator<T>::deallocate(T* p, unsigned long n) {}

template<typename T>
template<class... Args>
void StackAllocator<T>::construct(T *p, Args&&... args) {
    new((void*)p) T(std::forward<Args>(args)...);
}

template<typename T>
void StackAllocator<T>::destroy(T *p) {
    p->~T();
}
