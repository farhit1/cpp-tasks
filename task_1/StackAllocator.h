#ifndef CPPS_STACKALLOCATOR_H
#define CPPS_STACKALLOCATOR_H

#include <cstddef>
#include <memory>


class MemoryPool {
    typedef size_t      size_type;

    static const size_type          _blockSize;
    static std::unique_ptr<char[]>  _block;
    static size_type                _pos;

    template<typename T>
    friend class StackAllocator;

    MemoryPool() = delete;
};


template <typename T>
class StackAllocator;


template <>
class StackAllocator<void> {
public:
    typedef void*       pointer;
    typedef const void* const_pointer;
    typedef void        value_type;

    template <class U>
    struct rebind {
        typedef StackAllocator<U> other;
    };
};


template <typename T>
class StackAllocator {
public:
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T           value_type;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    template <class U>
    struct rebind {
        typedef StackAllocator<U> other;
    };

    StackAllocator();

    ~StackAllocator();

    pointer allocate(
            size_t n,
            StackAllocator<void>::const_pointer hint = 0
    );

    void deallocate(T* p, size_type n);

    template<class U, class... Args>
    void construct(U* p, Args&&... args);

    template<class U>
    void destroy(U* p);
};


#include "StackAllocator.cpp"

#endif //CPPS_STACKALLOCATOR_H
