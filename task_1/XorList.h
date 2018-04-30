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

        Node(const value_type& value, Node* xor_addr);
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
    Node* _getNode(U&& value) {
        Node* newNode = _nodeAllocator.allocate(1);
        _nodeAllocator.construct(newNode, std::forward<U>(value), _front);
        return newNode;
    }

    void _removeNode(Node* node) {
        _nodeAllocator.destroy(node);
        _nodeAllocator.deallocate(node, 1);
    }

public:
    explicit XorList(const Allocator& alloc = Allocator()) :
            _front(nullptr),
            _back(nullptr),
            _size(0),
            _allocator(alloc) {}
    explicit XorList(size_t count,
                     const value_type& value = value_type(),
                     const Allocator& alloc = Allocator()) :
            XorList(alloc) {
        while (count--)
            push_back(value);
    }


    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

    void clear() {
        while (!empty())
            pop_back();
    }


    XorList& operator=(const XorList& other) {
        if (&other == this)
            return *this;
        clear();
        for (auto it = other.begin(); it != other.end(); ++it)
            push_back(*it);
        return *this;
    }

    XorList& operator=(XorList&& other) noexcept {
        if (&other == this)
            return *this;
        _front = std::move(other._front);
        _back = std::move(other._back);
        _size = std::move(other._size);
        _allocator = std::move(other._allocator);
        return *this;
    }

    XorList(const XorList& other) :
            XorList() {
        operator=(other);
    }

    XorList(const XorList&& other) noexcept {
        operator=(std::move(other));
    }

    ~XorList() {
        clear();
    }


    template<typename U>
    void push_front(U&& value) {
        Node* newNode = _getNode(std::forward<U>(value));
        newNode->xor_addr = _front;
        if (_front != nullptr) {
            _front->xor_addr = _xor(_front->xor_addr, newNode);
            _front = newNode;
        } else
            _front = _back = newNode;
        ++_size;
    }

    template<typename U>
    void push_back(U&& value) {
        Node* newNode = _getNode(std::forward<U>(value));
        newNode->xor_addr = _back;
        if (_back != nullptr) {
            _back->xor_addr = _xor(_back->xor_addr, newNode);
            _back = newNode;
        } else
            _front = _back = newNode;
        ++_size;
    }

    void pop_front() {
        if (_size == 1)
            _front = _back = nullptr;
        else {
            Node* newFront = reinterpret_cast<Node*>(_front->xor_addr);
            newFront->xor_addr = _xor(newFront->xor_addr, _back);
            _removeNode(_front);
            _front = newFront;
        }
        --_size;
    }

    void pop_back() {
        if (_size == 1)
            _front = _back = nullptr;
        else {
            Node* newBack = reinterpret_cast<Node*>(_back->xor_addr);
            newBack->xor_addr = _xor(newBack->xor_addr, _back);
            _removeNode(_back);
            _back = newBack;
        }
        --_size;
    }


    class Iterator {
        Node* _prevNode;
        Node* _node;
        Node* _nextNode;

    public:
        Iterator(Node* prevNode, Node* node, Node* nextNode) :
                _prevNode(prevNode),
                _node(node),
                _nextNode(nextNode) {}

        Iterator& operator=(const Iterator& iterator) {
            _prevNode = iterator._prevNode;
            _node = iterator._node;
            _nextNode = iterator._nextNode;
            return *this;
        }

        Iterator& operator++() {
            Node* newNextNode = _xor(_nextNode == nullptr ? nullptr : _nextNode->xor_addr,
                                     _node);
            _prevNode = _node;
            _node = _nextNode;
            _nextNode = newNextNode;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            operator++();
            return tmp;
        }

        Iterator& operator--() {
            Node* newPrevNode = _xor(_prevNode == nullptr ? nullptr : _prevNode->xor_addr,
                                     _node);
            _nextNode = _node;
            _node = _prevNode;
            _prevNode = newPrevNode;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp(*this);
            operator--();
            return tmp;
        }

        value_type& operator*() const {
            return _node->value;
        }

        value_type* operator->() const {
            return _node;
        }

        bool operator==(const Iterator& other) const {
            return _node == other._node;
        }

        bool operator!=(const Iterator& other) const {
            return !operator==(other);
        }

        ~Iterator() = default;

        template<typename T_, typename Allocator_>
        friend class XorList;
    };

    Iterator begin() const {
        return Iterator(nullptr, _front,
                        _front == nullptr ? nullptr : reinterpret_cast<Node*>(_front->xor_addr));
    }

    Iterator end() const {
        return Iterator(_back, nullptr, nullptr);
    }

    template<typename U>
    void insert_before(Iterator iterator, U&& value) {
        Node* newNode = _getNode(std::forward<U>(value));
        newNode->xor_addr = _xor(iterator._prevNode, iterator._node);
        if (empty())
            _front = _back = newNode;
        if (iterator._node != nullptr) {
            iterator._node->xor_addr = _xor(iterator._node->xor_addr, iterator._prevNode);
            iterator._node->xor_addr = _xor(iterator._node->xor_addr, newNode);
        }
        if (iterator._prevNode != nullptr) {
            iterator._prevNode->xor_addr = _xor(iterator._prevNode->xor_addr, iterator._node);
            iterator._prevNode->xor_addr = _xor(iterator._prevNode->xor_addr, newNode);
        }
        if (iterator._node == _front)
            _front = newNode;
        if (iterator._prevNode == _back)
            _back = newNode;
        ++_size;
    }

    template<typename U>
    void insert_after(Iterator iterator, U&& value) {
        Node* newNode = _getNode(std::forward<U>(value));
        newNode->xor_addr = _xor(iterator._nextNode, iterator._node);
        if (empty())
            _front = _back = newNode;
        if (iterator._node != nullptr) {
            iterator._node->xor_addr = _xor(iterator._node->xor_addr, iterator._nextNode);
            iterator._node->xor_addr = _xor(iterator._node->xor_addr, newNode);
        }
        if (iterator._nextNode != nullptr) {
            iterator._nextNode->xor_addr = _xor(iterator._nextNode->xor_addr, iterator._node);
            iterator._nextNode->xor_addr = _xor(iterator._nextNode->xor_addr, newNode);
        }
        if (iterator._nextNode == _front)
            _front = newNode;
        if (iterator._node == _back)
            _back = newNode;
        ++_size;
    }

    void erase(Iterator iterator) {
        --_size;
        if (iterator._prevNode != nullptr) {
            iterator._prevNode->xor_addr = _xor(iterator._prevNode->xor_addr, iterator._node);
            iterator._prevNode->xor_addr = _xor(iterator._prevNode->xor_addr, iterator._nextNode);
        }
        if (iterator._nextNode != nullptr) {
            iterator._nextNode->xor_addr = _xor(iterator._nextNode->xor_addr, iterator._node);
            iterator._nextNode->xor_addr = _xor(iterator._nextNode->xor_addr, iterator._prevNode);
        }
        if (_front == iterator._node)
            _front = iterator._nextNode;
        if (_back == iterator._node)
            _back = iterator._prevNode;
        _removeNode(iterator._node);
    }

#if __has_include("vector")
    std::vector<value_type> toVector() const {
        std::vector<value_type> tmp;
        tmp.reserve(size());
        for (auto it = begin(); it != end(); ++it)
            tmp.push_back(*it);
        return tmp;
    }

    operator std::vector<value_type>() const {
        return toVector();
    }
#endif
};

#include "XorList.cpp"

#endif //CPPS_XORLIST_H
