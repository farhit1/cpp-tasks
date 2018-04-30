#ifndef CPPS_STACKALLOCATOR_H
#define CPPS_STACKALLOCATOR_H

#include <cstddef>
#include <memory>

class MemoryPool {
    typedef size_t      size_type;

    static const size_type blockSize = 1e7;

    static std::unique_ptr<char[]>      block;
    static size_type                    pos;

    struct BlockNode {
        std::unique_ptr<char[]> block;
        std::unique_ptr<BlockNode> nextBlock;
    };

    static std::unique_ptr<BlockNode>   prevBlock;

    static void* blockGet(size_type n);

public:
    static void* get(size_type n);

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
