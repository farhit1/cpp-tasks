#ifndef CPPS_XORLIST_H
#define CPPS_XORLIST_H

#include <cstddef>
#include <memory>


template<typename T, typename Allocator = std::allocator<T>>
class XorList {
    typedef T           value_type;
    typedef intptr_t    intptr;
    static_assert(std::is_same<typename Allocator::value_type, value_type>::value);

    struct Node {
        value_type  value;
        Node*       xor_addr;

        Node(const value_type&  value, Node* xor_addr);
        Node(const value_type&& value, Node* xor_addr);
        ~Node() = default;
    };

    static Node* _xor(const Node* lhs, const Node* rhs);

    Node*   _front;
    Node*   _back;
    size_t  _size;

    Allocator                                           _allocator;
    typename Allocator::template rebind<Node>::other    _nodeAllocator;

    template<typename U>
    Node* _getNode(U&& value);

    void _removeNode(Node* node);

public:
    explicit XorList(const Allocator& alloc = Allocator());

    explicit XorList(size_t count,
                     const value_type& value = value_type(),
                     const Allocator& alloc = Allocator());


    size_t size() const;

    bool empty() const;

    void clear();


    XorList& operator=(const XorList& other);

    XorList& operator=(XorList&& other) noexcept;

    XorList(const XorList& other);

    XorList(const XorList&& other) noexcept;

    ~XorList();


    template<typename U>
    void push_front(U&& value);

    template<typename U>
    void push_back(U&& value);

    void pop_front();

    void pop_back();


    class Iterator {
        Node* _prevNode;
        Node* _node;

        Node* _getNextNode() const;

    public:
        Iterator(Node* prevNode, Node* node);

        Iterator& operator=(const Iterator& iterator);

        Iterator& operator++();

        Iterator operator++(int);

        Iterator& operator--();

        Iterator operator--(int);

        value_type& operator*() const;

        value_type* operator->() const;

        bool operator==(const Iterator& other) const;

        bool operator!=(const Iterator& other) const;

        ~Iterator() = default;

        template<typename T_, typename Allocator_>
        friend class XorList;
    };

    Iterator begin() const;

    Iterator end() const;

    template<typename U>
    void insert_before(Iterator iterator, U&& value);

    template<typename U>
    void insert_after(Iterator iterator, U&& value);

    void erase(Iterator iterator);

#if __has_include("vector")
    operator std::vector<value_type>() const;
#endif
};


#include "XorList.cpp"

#endif //CPPS_XORLIST_H
