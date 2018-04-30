#include "XorList.h"

template<typename T_, typename Allocator_>
XorList<T_, Allocator_>::Node::Node(const value_type& value, Node* xor_addr) :
        value(value),
        xor_addr(xor_addr) {}

template<typename T_, typename Allocator_>
XorList<T_, Allocator_>::Node::Node(const value_type&& value, Node* xor_addr) :
        value(std::move(value)),
        xor_addr(xor_addr) {}

template<typename T_, typename Allocator_>
typename XorList<T_, Allocator_>::Node* _xor(const typename XorList<T_, Allocator_>::Node* lhs,
                                             const typename XorList<T_, Allocator_>::Node* rhs) {
    return reinterpret_cast<Node*>(
            reinterpret_cast<intptr>(lhs) ^
            reinterpret_cast<intptr>(rhs)
    );
}
