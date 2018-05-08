#include <iostream>

#include <tuple>



template<class... Types>
class Tuple;

// non-empty tuple
template<class T, class... Tail>
class Tuple<T, Tail...> {
    T _value;
    Tuple<Tail...> *_tail;

    friend class TupleHelper;

public:
    Tuple() :
            _value(std::move(T())),
            _tail(new Tuple<Tail...>()) {}

    template<typename U, typename... Args>
    explicit Tuple(const U& arg, Args&&... args) :
            _value(arg),
            _tail(new Tuple<Args...>(std::forward<Args>(args)...)) {}

    template<typename U, typename... Args>
    explicit Tuple(const U&& arg, Args&&... args) :
            _value(std::move(arg)),
            _tail(new Tuple<Args...>(std::forward<Args>(args)...)) {};

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

    Tuple(const Tuple<T, Tail...>& other) {
        operator=(other);
    }

    Tuple(Tuple<T, Tail...>&& other) noexcept {
        operator=(std::forward<Tuple<T, Tail...>>(other));
    }

    ~Tuple() {
        delete _tail;
    }

    template<typename U>
    void swap(U&& other) {
        std::swap(_value, other._value);
        _tail->swap(std::forward<Tuple<Tail...>>(*other._tail));
    }
};


// empty tuple
template<>
class Tuple<> {
    friend class TupleHelper;

    struct _empty_type {};

public:
    typedef _empty_type type;

    Tuple() = default;
    ~Tuple() = default;

    template<typename U>
    void swap(U&& other) {
        static_assert(std::is_same<_empty_type, typename U::type>::value);
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


    template<typename T, typename U>
    struct concat_two;

    template<typename... Ts, typename... Us>
    struct concat_two<Tuple<Ts...>, Tuple<Us...>> {
        typedef Tuple<Ts..., Us...> type;

        template<typename T, typename U>
        static type value(T&& t, U&& u) {
            return type(); // !!!
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
            return type(t);
        }
    };

    template<typename T, typename U, typename... Tail>
    struct concat<T, U, Tail...> {
        typedef typename concat<typename concat_two<T, U>::type, Tail...>::type type;

        template<typename T1, typename U1, typename... Tail1>
        static type value(T1&& t, U1&& u, Tail1&&... tail) {
            static_assert(std::is_same<T, T1>::value);
            static_assert(std::is_same<U, U1>::value);
            return concat::value(concat_two<T, U>::value(std::forward<T1>(t), std::forward<U1>(u)),
                                 std::forward<Tail1>(tail)...);
        }
    };
};


template<class... Args>
Tuple<Args...> makeTuple(Args&&... args) {
    return Tuple<Args...>(std::forward<Args>(args)...);
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
    return TupleHelper::concat<typename std::remove_reference<Tuples>::type...>::value(
            std::forward<Tuples>(tuples)...
    );
}



int main() {
    int a = 5;
    Tuple<int, int> s(a, 10);

    auto f = tupleCat(s, makeTuple(1, std::string(), 3), s);

    auto x = makeTuple(43, 433);
//    decltype(f)::type d;

//    std::cout << get<std::string>(x);

    std::cout << get<0>(f) << std::endl;

    return 0;
}
