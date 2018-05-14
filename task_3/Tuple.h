#ifndef TUPLE_H
#define TUPLE_H

#include <type_traits>
#include <array>

template<typename... T>
class Tuple;


// empty tuple
template<>
class Tuple<> {
    friend class TupleHelper;
    template<typename... T>
    friend class Tuple;

    struct _empty_tuple_type {
        static const bool value = true;
    };
    struct _inner_tuple_type {};

public:
    typedef _empty_tuple_type type;

    Tuple() = default;
    ~Tuple() = default;

    template<typename U>
    void swap(U&& other) {
        static_assert(std::is_same<typename std::remove_reference<U>::type::_empty_tuple_type,
                _empty_tuple_type>::value);
    }
};


// non-empty tuple
template<class T, class... Tail>
class Tuple<T, Tail...> {
    friend class TupleHelper;

    struct _empty_tuple_type {
        static const bool value = false;
    };
    struct _inner_tuple_type {};

    T _value;
    Tuple<Tail...> *_tail;

    template<typename U, typename T_>
    static Tuple<T, Tail...> _pushFront(U&& value, T_&& tuple) {
        Tuple<T, Tail...> t(value, (typename Tuple<>::_inner_tuple_type()));
        t._tail = new Tuple<Tail...>(std::forward<Tuple<Tail...>>(tuple));
        return t;
    }

    template<typename U, typename... Args>
    Tuple(const U& value, Tuple<>::_inner_tuple_type _) :
            _value(value) {};

public:
    Tuple() :
            _value(std::move(T())),
            _tail(new Tuple<Tail...>()) {}

    template<typename U, typename... Args>
    explicit Tuple(const U& arg, Args&&... args) :
            _value(arg),
            _tail(new Tuple<Tail...>(std::forward<Tail>(args)...)) {}

    template<typename U, typename... Args>
    explicit Tuple(const U&& arg, Args&&... args) :
            _value(std::move(arg)),
            _tail(new Tuple<Tail...>(std::forward<Tail>(args)...)) {}

    Tuple& operator=(const Tuple<T, Tail...>& other) {
        _value = other._value;
        _tail->operator=(*other._tail);
        return *this;
    }

    Tuple& operator=(Tuple<T, Tail...>&& other) noexcept {
        _value = std::move(other._value);
        _tail = other._tail;
        other._tail = nullptr;
        return *this;
    }

    Tuple(const Tuple<T, Tail...>& other) :
            Tuple() {
        operator=(other);
    }

    Tuple(Tuple<T, Tail...>&& other) noexcept {
        operator=(std::forward<Tuple<T, Tail...>>(other));
    }

    ~Tuple() {
        if (_tail)
            delete _tail;
    }

    template<typename U>
    void swap(U&& other) {
        std::swap(_value, other._value);
        _tail->swap(std::forward<Tuple<Tail...>>(*other._tail));
    }
};


struct TupleHelper {
    template<size_t i, typename T, typename... Args>
    struct get {
        typedef typename get<i - 1, Args...>::type type;

        static type& value(Tuple<T, Args...>& tuple) {
            return get<i - 1, Args...>::value(*tuple._tail);
        }

        static const type& value(const Tuple<T, Args...>& tuple) {
            return get<i - 1, Args...>::value(const_cast<const Tuple<Args...>&>(*tuple._tail));
        }

        static type&& value(Tuple<T, Args...>&& tuple) {
            return get<i - 1, Args...>::value(std::forward<Tuple<Args...>>(*tuple._tail));
        }
    };

    template<typename T, typename... Args>
    struct get<0, T, Args...> {
        typedef T type;

        static type& value(Tuple<T, Args...>& tuple) {
            return tuple._value;
        }

        static const type& value(const Tuple<T, Args...>& tuple) {
            return tuple._value;
        }

        static type&& value(Tuple<T, Args...>&& tuple) {
            return std::forward<T>(tuple._value);
        }
    };


    template<typename Q, typename T, typename... Args>
    struct getByType {
        typedef Q type;

        static type& value(Tuple<T, Args...>& tuple) {
            return getByType<Q, Args...>::value(*tuple._tail);
        }

        static const type& value(const Tuple<T, Args...>& tuple) {
            return getByType<Q, Args...>::value(const_cast<const Tuple<Args...>&>(*tuple._tail));
        }

        static type&& value(Tuple<T, Args...>&& tuple) {
            return getByType<Q, Args...>::value(std::forward<Tuple<Args...>>(*tuple._tail));
        }
    };

    template<typename T, typename... Args>
    struct getByType<T, T, Args...> {
        typedef T type;

        static type& value(Tuple<T, Args...>& tuple) {
            return tuple._value;
        }

        static const type& value(const Tuple<T, Args...>& tuple) {
            return tuple._value;
        }

        static type&& value(Tuple<T, Args...>&& tuple) {
            return std::forward<T>(tuple._value);
        }
    };


    template<typename... Args>
    struct reverse;

    template<typename... Ts>
    struct reverse<Tuple<Ts...>, Tuple<>> {
        typedef Tuple<Ts...> type;

        static type value(Tuple<Ts...>&& t, Tuple<> u) {
            return std::move(t);
        }
    };

    template<typename... Ts, typename U1, typename... Us>
    struct reverse<Tuple<Ts...>, Tuple<U1, Us...>> {
        typedef typename reverse<Tuple<U1, Ts...>, Tuple<Us...>>::type type;

        template<typename U_>
        static type value(Tuple<Ts...>&& t, U_&& u) {
            Tuple<U1, Ts...> newTuple(Tuple<U1, Ts...>::_pushFront(TupleHelper::get<0, U1, Us...>::value(u), t));

            return reverse<Tuple<U1, Ts...>, Tuple<Us...>>::value(
                    std::move(newTuple),
                    std::forward<Tuple<Us...>>(*u._tail)
            );
        }
    };

    template<typename... Args>
    struct concat {
        typedef Tuple<> type;

        static type value() {
            return type();
        }
    };

    template<typename... Ts>
    struct concat<Tuple<Ts...>> {
        typedef Tuple<Ts...> type;

        static type value(Tuple<Ts...>&& t) {
            return std::move(t);
        }
    };

    template<typename... Ts, typename... Tail>
    struct concat<Tuple<Ts...>, Tuple<>, Tail...> {
        typedef typename concat<Tuple<Ts...>, Tail...>::type type;

        template<typename... Tail_>
        static type value(Tuple<Ts...>&& t, Tuple<> u, Tail_&&... tail) {
            return concat<Tuple<Ts...>, Tail...>::value(
                    std::forward<Tuple<Ts...>>(t),
                    std::forward<Tail>(tail)...
            );
        }
    };

    template<typename... Ts, typename U1, typename... Us, typename... Tail>
    struct concat<Tuple<Ts...>, Tuple<U1, Us...>, Tail...> {
        typedef typename concat<Tuple<U1, Ts...>, Tuple<Us...>, Tail...>::type type;

        template<typename U_, typename... Tail_>
        static type value(Tuple<Ts...>&& t, U_&& u, Tail_&&... tail) {
            Tuple<U1, Ts...> newTuple(Tuple<U1, Ts...>::_pushFront(TupleHelper::get<0, U1, Us...>::value(u), t));

            return concat<Tuple<U1, Ts...>, Tuple<Us...>, Tail...>::value(
                    std::move(newTuple),
                    std::forward<Tuple<Us...>>(*u._tail),
                    std::forward<Tail>(tail)...
            );
        }
    };

    enum class cmp {
        less, equal, more
    };
    template<typename T_, typename U_>
    static cmp compare_values(const T_& t, const U_& u) {
        static_assert(std::is_same<typename std::remove_cv<typename std::remove_reference<T_>::type>::type,
                typename std::remove_cv<typename std::remove_reference<T_>::type>::type>::value);
        if (t._value < u._value)
            return cmp::less;
        if (t._value == u._value)
            return cmp::equal;
        if (t._value > u._value)
            return cmp::more;
    }

    template<bool b, typename T_, typename U_>
    static bool compare(const T_& t, const U_& u, cmp by) {
        if (T_::_empty_tuple_type::value == true)
            return by == cmp::equal;
        switch(TupleHelper::compare_values(t, u)) {
            case TupleHelper::cmp::less:
                return by == cmp::less;
            case TupleHelper::cmp::equal:
                return compare<true>(*t._tail, *u._tail, by);
            case TupleHelper::cmp::more:
                return by == cmp::more;
        }
    }

    template<bool b>
    static bool compare(const Tuple<>& t, const Tuple<>& u, cmp by) {
        return by == cmp::equal;
    }
};


template<class... Args>
Tuple<typename std::decay<Args>::type...> makeTuple(Args&&... args) {
    return Tuple<typename std::decay<Args>::type...>(std::forward<Args>(args)...);
}


template<size_t i, typename... Args>
typename TupleHelper::template get<i, Args...>::type& get(Tuple<Args...>& tuple) {
    return TupleHelper::get<i, Args...>::value(tuple);
}

template<size_t i, typename... Args>
const typename TupleHelper::template get<i, Args...>::type& get(const Tuple<Args...>& tuple) {
    return TupleHelper::get<i, Args...>::value(tuple);
}

template<size_t i, typename... Args>
typename TupleHelper::template get<i, Args...>::type&& get(Tuple<Args...>&& tuple) {
    return TupleHelper::get<i, Args...>::value(std::forward<Tuple<Args...>>(tuple));
}


template<typename T, typename... Args>
typename TupleHelper::template getByType<T, Args...>::type& get(Tuple<Args...>& tuple) {
    return TupleHelper::getByType<T, Args...>::value(tuple);
};

template<typename T, typename... Args>
const typename TupleHelper::template getByType<T, Args...>::type& get(const Tuple<Args...>& tuple) {
    return TupleHelper::getByType<T, Args...>::value(tuple);
};

template<typename T, typename... Args>
typename TupleHelper::template getByType<T, Args...>::type&& get(Tuple<Args...>&& tuple) {
    return TupleHelper::getByType<T, Args...>::value(std::forward<Tuple<Args...>>(tuple));
};


template<typename... Tuples>
auto tupleCat(Tuples&&... tuples) {
    typedef TupleHelper::concat<Tuple<>, typename std::remove_reference<Tuples>::type...> _rev_tuple;
    return TupleHelper::reverse<Tuple<>, typename _rev_tuple::type>::value(Tuple<>(), _rev_tuple::value(
            Tuple<>(),
            std::forward<Tuples>(tuples)...
    ));
}


template<typename... Args>
bool operator<(const Tuple<Args...>& t, const Tuple<Args...>& u) {
    return TupleHelper::compare<true>(t, u, TupleHelper::cmp::less);
}

template<typename... Args>
bool operator==(const Tuple<Args...>& t, const Tuple<Args...>& u) {
    return TupleHelper::compare<true>(t, u, TupleHelper::cmp::equal);
}

template<typename... Args>
bool operator>(const Tuple<Args...>& t, const Tuple<Args...>& u) {
    return TupleHelper::compare<true>(t, u, TupleHelper::cmp::more);
}

template<typename... Args>
bool operator<=(const Tuple<Args...>& t, const Tuple<Args...>& u) {
    return TupleHelper::compare<true>(t, u, TupleHelper::cmp::less) ||
           TupleHelper::compare<true>(t, u, TupleHelper::cmp::equal);
}

template<typename... Args>
bool operator>=(const Tuple<Args...>& t, const Tuple<Args...>& u) {
    return TupleHelper::compare<true>(t, u, TupleHelper::cmp::more) ||
           TupleHelper::compare<true>(t, u, TupleHelper::cmp::equal);
}

template<typename... Args>
bool operator!=(const Tuple<Args...>& t, const Tuple<Args...>& u) {
    return !TupleHelper::compare<true>(t, u, TupleHelper::cmp::equal);
}


#endif //CPPS_TUPLE_H
