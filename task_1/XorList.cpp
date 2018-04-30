#include "XorList.h"

template<typename T, typename Allocator>
XorList<T, Allocator>::Node::Node(const value_type& value, Node* xor_addr) :
        value(value),
        xor_addr(xor_addr) {}

template<typename T, typename Allocator>
XorList<T, Allocator>::Node::Node(const value_type&& value, Node* xor_addr) :
        value(std::move(value)),
        xor_addr(xor_addr) {}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Node*
XorList<T, Allocator>::_xor(const Node* lhs, const Node* rhs) {
    return reinterpret_cast<Node*>(
            reinterpret_cast<intptr>(lhs) ^
            reinterpret_cast<intptr>(rhs)
    );
}

template<typename T, typename Allocator>
template<typename U>
typename XorList<T, Allocator>::Node*
XorList<T, Allocator>::_getNode(U&& value) {
    Node* newNode = _nodeAllocator.allocate(1);
    _nodeAllocator.construct(newNode, std::forward<U>(value), _front);
    return newNode;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::_removeNode(Node* node) {
    _nodeAllocator.destroy(node);
    _nodeAllocator.deallocate(node, 1);
}

template<typename T, typename Allocator>
XorList<T, Allocator>::XorList(const Allocator& alloc) :
        _front(nullptr),
        _back(nullptr),
        _size(0),
        _allocator(alloc) {}

template<typename T, typename Allocator>
XorList<T, Allocator>::XorList(size_t count,
                               const value_type& value,
                               const Allocator& alloc) :
        XorList(alloc) {
    while (count--)
        push_back(value);
}

template<typename T, typename Allocator>
size_t XorList<T, Allocator>::size() const {
    return _size;
}

template<typename T, typename Allocator>
bool XorList<T, Allocator>::empty() const {
    return _size == 0;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::clear() {
    while (!empty())
        pop_back();
}

template<typename T, typename Allocator>
XorList<T, Allocator>&
XorList<T, Allocator>::operator=(const XorList& other) {
    if (&other == this)
        return *this;
    clear();
    for (auto it = other.begin(); it != other.end(); ++it)
        push_back(*it);
    return *this;
}

template<typename T, typename Allocator>
XorList<T, Allocator>&
XorList<T, Allocator>::operator=(XorList&& other) noexcept {
    if (&other == this)
        return *this;
    _front = std::move(other._front);
    _back = std::move(other._back);
    _size = std::move(other._size);
    _allocator = std::move(other._allocator);
    return *this;
}

template<typename T, typename Allocator>
XorList<T, Allocator>::XorList(const XorList& other) :
        XorList() {
    operator=(other);
}

template<typename T, typename Allocator>
XorList<T, Allocator>::XorList(const XorList&& other) noexcept {
    operator=(std::move(other));
}

template<typename T, typename Allocator>
XorList<T, Allocator>::~XorList() {
    clear();
}

template<typename T, typename Allocator>
template<typename U>
void XorList<T, Allocator>::push_front(U&& value) {
    Node* newNode = _getNode(std::forward<U>(value));
    newNode->xor_addr = _front;
    if (_front != nullptr) {
        _front->xor_addr = _xor(_front->xor_addr, newNode);
        _front = newNode;
    } else
        _front = _back = newNode;
    ++_size;
}

template<typename T, typename Allocator>
template<typename U>
void XorList<T, Allocator>::push_back(U&& value) {
    Node* newNode = _getNode(std::forward<U>(value));
    newNode->xor_addr = _back;
    if (_back != nullptr) {
        _back->xor_addr = _xor(_back->xor_addr, newNode);
        _back = newNode;
    } else
        _front = _back = newNode;
    ++_size;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::pop_front() {
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

template<typename T, typename Allocator>
void XorList<T, Allocator>::pop_back() {
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

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Node*
XorList<T, Allocator>::Iterator::_getNextNode() const {
    return _xor(_node == nullptr ? nullptr : _node->xor_addr,
                _prevNode);
}

template<typename T, typename Allocator>
XorList<T, Allocator>::Iterator::Iterator(Node* prevNode, Node* node) :
        _prevNode(prevNode),
        _node(node) {}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Iterator&
XorList<T, Allocator>::Iterator::operator=(const Iterator& iterator) {
    _prevNode = iterator._prevNode;
    _node = iterator._node;
    return *this;
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Iterator&
XorList<T, Allocator>::Iterator::operator++() {
    Node* nextNode = _getNextNode();
    _prevNode = _node;
    _node = nextNode;
    return *this;
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Iterator
XorList<T, Allocator>::Iterator::operator++(int) {
    Iterator tmp(*this);
    operator++();
    return tmp;
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Iterator&
XorList<T, Allocator>::Iterator::operator--() {
    Node* newPrevNode = _xor(
            _prevNode == nullptr ? nullptr : _prevNode->xor_addr,
            _node);
    _node = _prevNode;
    _prevNode = newPrevNode;
    return *this;
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Iterator
XorList<T, Allocator>::Iterator::operator--(int) {
    Iterator tmp(*this);
    operator--();
    return tmp;
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::value_type&
XorList<T, Allocator>::Iterator::operator*() const {
    return _node->value;
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::value_type*
XorList<T, Allocator>::Iterator::operator->() const {
    return _node;
}

template<typename T, typename Allocator>
bool
XorList<T, Allocator>::Iterator::operator==(const Iterator& other) const {
    return _node == other._node;
}

template<typename T, typename Allocator>
bool
XorList<T, Allocator>::Iterator::operator!=(const Iterator& other) const {
    return !operator==(other);
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Iterator
XorList<T, Allocator>::begin() const {
    return Iterator(nullptr, _front);
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Iterator
XorList<T, Allocator>::end() const {
    return Iterator(_back, nullptr);
}

template<typename T, typename Allocator>
template<typename U>
void XorList<T, Allocator>::insert_before(Iterator iterator, U&& value) {
    Node* newNode = _getNode(std::forward<U>(value));
    newNode->xor_addr = _xor(iterator._prevNode, iterator._node);
    if (empty())
        _front = _back = newNode;
    if (iterator._node != nullptr) {
        iterator._node->xor_addr = _xor(
                iterator._node->xor_addr,
                iterator._prevNode
        );
        iterator._node->xor_addr = _xor(
                iterator._node->xor_addr,
                newNode
        );
    }
    if (iterator._prevNode != nullptr) {
        iterator._prevNode->xor_addr = _xor(
                iterator._prevNode->xor_addr,
                iterator._node
        );
        iterator._prevNode->xor_addr = _xor(
                iterator._prevNode->xor_addr,
                newNode
        );
    }
    if (iterator._node == _front)
        _front = newNode;
    if (iterator._prevNode == _back)
        _back = newNode;
    ++_size;
}

template<typename T, typename Allocator>
template<typename U>
void XorList<T, Allocator>::insert_after(Iterator iterator, U&& value) {
    Node* newNode = _getNode(std::forward<U>(value));
    Node* nextNode = iterator._getNextNode();
    newNode->xor_addr = _xor(nextNode, iterator._node);
    if (empty())
        _front = _back = newNode;
    if (iterator._node != nullptr) {
        iterator._node->xor_addr = _xor(iterator._node->xor_addr, nextNode);
        iterator._node->xor_addr = _xor(iterator._node->xor_addr, newNode);
    }
    if (nextNode != nullptr) {
        nextNode->xor_addr = _xor(nextNode->xor_addr, iterator._node);
        nextNode->xor_addr = _xor(nextNode->xor_addr, newNode);
    }
    if (nextNode == _front)
        _front = newNode;
    if (iterator._node == _back)
        _back = newNode;
    ++_size;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::erase(Iterator iterator) {
    --_size;
    Node* nextNode = iterator._getNextNode();
    if (iterator._prevNode != nullptr) {
        iterator._prevNode->xor_addr = _xor(
                iterator._prevNode->xor_addr,
                iterator._node
        );
        iterator._prevNode->xor_addr = _xor(
                iterator._prevNode->xor_addr,
                nextNode
        );
    }
    if (nextNode != nullptr) {
        nextNode->xor_addr = _xor(nextNode->xor_addr, iterator._node);
        nextNode->xor_addr = _xor(nextNode->xor_addr, iterator._prevNode);
    }
    if (_front == iterator._node)
        _front = nextNode;
    if (_back == iterator._node)
        _back = iterator._prevNode;
    _removeNode(iterator._node);
}

#if __has_include("vector")
template<typename T, typename Allocator>
XorList<T, Allocator>::operator std::vector<value_type>() const {
    std::vector<value_type> tmp;
    tmp.reserve(size());
    for (auto it = begin(); it != end(); ++it)
        tmp.push_back(*it);
    return tmp;
}
#endif
