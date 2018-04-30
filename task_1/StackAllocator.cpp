#include "StackAllocator.h"

template <typename T>
const size_t StackAllocator<T>::_blockSize = 2e8;

template <typename T>
std::unique_ptr<char[]> StackAllocator<T>::_block(new char[StackAllocator<T>::_blockSize]);

template <typename T>
size_t StackAllocator<T>::_pos = 0;

template <typename T>
StackAllocator<T>::StackAllocator() {}

template <typename T>
StackAllocator<T>::~StackAllocator() {}

template <typename T>
typename StackAllocator<T>::pointer StackAllocator<T>::allocate(typename StackAllocator<int>::size_type n,
                                                                typename StackAllocator<void>::const_pointer hint)
{
    pointer start = reinterpret_cast<pointer>(&_block[_pos]);
    _pos += n * sizeof(value_type);
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
