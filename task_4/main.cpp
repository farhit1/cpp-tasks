#include "ahalaimahalai.cpp"
#include <iostream>

// SOME CONSTANTS

constexpr int Zero      = 0;
constexpr int One       = 1;
constexpr int Two       = One + One;
constexpr int Twenty    = Two * Two * Two * Two + Two * Two;   // ceil(log_2(1e6))


// IF IMPLEMENTATION
// usage: If<statement>::value(trueValue, falseValue)

template<bool statement>
struct If {
    template<typename T>
    static constexpr T value(T ifTrue, T ifFalse) {
        return ifTrue;
    }
};

template<>
struct If<false> {
    template<typename T>
    static constexpr T value(T ifTrue, T ifFalse) {
        return ifFalse;
    }
};


// FIND y: 2^(y+1) > x >= 2^y
// usage: OlderBit<x>

template<int number, int pos = Twenty>
struct OlderBit {
    static constexpr int value = If<(number >> pos) & One>::value(
            pos,
            OlderBit<number & (((One << Twenty) - One) ^ (One << pos)), pos - One>::value
    );
};

template<>
struct OlderBit<Zero, Zero> {
    static constexpr int value = -One;
};

template<>
struct OlderBit<One, Zero> {
    static constexpr int value = Zero;
};



// FIND POSITION AND HOW MANY ELEMENTS
// FIRST PLAYER SHOULD TAKE FROM IT TO WIN
// members: int position
//          int take

template<int index, int xorsum, int... restOfVals>
struct AhalaiMahalaiFindMove;

template<int index, int xorsum>
struct AhalaiMahalaiFindMove<index, xorsum> {
    static constexpr int position = -One;
    static constexpr int take = -One;
};

template<int index, int xorsum, int val, int... restOfVals>
struct AhalaiMahalaiFindMove<index, xorsum, val, restOfVals...> {
    static constexpr int position = If<OlderBit<val>::value >= OlderBit<xorsum>::value>::value(
            index,
            AhalaiMahalaiFindMove<index + One, xorsum, restOfVals...>::position
    );
    static constexpr int take = If<OlderBit<val>::value >= OlderBit<xorsum>::value>::value(
            val - (xorsum ^ val),
            AhalaiMahalaiFindMove<index + One, xorsum, restOfVals...>::take
    );
};


// FIND WINNER AND XOR OF ALL VALUES
// members: int winner
//          int xorsum

template<int xorsum, int... vals>
struct Winner;

template<int xsum>
struct Winner<xsum> {
    static constexpr int winner = If<xsum != Zero>::value(One, Two);
    static constexpr int xorsum = xsum;
};

template<int xsum, int val, int... restOfVals>
struct Winner<xsum, val, restOfVals...> {
    static constexpr int winner = Winner<xsum ^ val, restOfVals...>::winner;
    static constexpr int xorsum = Winner<xsum ^ val, restOfVals...>::xorsum;
};


// STORES ANSWERS

template<int... vals>
struct AhalaiMahalai {
    static constexpr int who = Winner<Zero, vals...>::winner;
    static constexpr int whence = If<who == One>::value(
            AhalaiMahalaiFindMove<One, Winner<Zero, vals...>::xorsum, vals...>::position,
            Zero
    );
    static constexpr int how = If<who == One>::value(
            AhalaiMahalaiFindMove<One, Winner<Zero, vals...>::xorsum, vals...>::take,
            Zero
    );
};


int main() {
    constexpr int who = AhalaiMahalai<1, 17, 993, 149552>::who;
    constexpr int whence = AhalaiMahalai<1, 17, 993, 149552>::whence;
    constexpr int how = AhalaiMahalai<1, 17, 993, 149552>::how;
    std::cout << who << ' ' << whence << ' ' << how;
}
