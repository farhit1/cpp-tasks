# 2nd semester, `C++` tasks

[**Statements** *(russian)*](https://docs.google.com/document/d/1STYwaHqLIn2di5290iGjULACqIRExZl7647w9m21wnc/edit)

## 1. [XOR-List](https://github.com/farhit1/cpp-tasks/tree/master/task_1)

Implement `StackAllocator<T>`, which allocates single large piece of memory and returns memory for objects from it and [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list) to check how optimal new allocation by time is.

## 2. [Global allocator switching](https://github.com/farhit1/mipt2-cpp/tree/master/task_2)

Implement system of global allocator switching. At a particular moment there should be only one active allocator and calling `new` should use this allocator, not `malloc`, as `new` does by default. All the allocators should be inherited from `IMemoryManager`:
```c++
class IMemoryManager {
public:
    virtual void* Alloc(size_t size) = 0;
    virtual void Free(void* ptr) = 0;
};
```
Problems:
- object, allocated on a single allocator, should be deallocated on it, even if another allocator is active now;
- implement a mechanism to switch allocator using RAII concept;
- implement a strategy to allocate some classes are on their own allocators.

## 3. [Tuple](https://github.com/farhit1/mipt2-cpp/tree/master/task_3)
## 4. [Templated thinking](https://github.com/farhit1/mipt2-cpp/tree/master/task_4)

Calculate winner of nim game and his first move during compilation time.
```c++
int main() {
    constexpr int who = AhalaiMahalai<N_1, N_2, …, N_k>::who;
    constexpr int whence = AhalaiMahalai<N_1, N_2, …, N_k>::whence;
    constexpr int how = AhalaiMahalai<N_1, N_2, …, N_k>::how;
    std::cout << who << ‘ ‘ << whence << ‘ ‘ << how;
}
```
