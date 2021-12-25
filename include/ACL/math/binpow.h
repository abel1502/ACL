#ifndef ABEL_MATH_BINPOW_H
#define ABEL_MATH_BINPOW_H

#include <ACL/general.h>
#include <ACL/type_traits.h>
#include <ACL/math/getneutral.h>


namespace abel::math {


template <typename T>
constexpr T binPow(T base, unsigned power) {
    T result = getMulNeutral<T>();

    while (power) {
        if (power & 1) {
            result *= base;
            --power;
        }

        base *= base;  // TODO: Validate correctness
        power >>= 1;
    }

    return result;
}


template <typename T, typename U = T>
constexpr T binPow(T base, unsigned power, U mod) {
    T result = getMulNeutral<T>();

    while (power) {
        if (power & 1) {
            result = result * base % mod;
            --power;
        }

        base = base * base % mod;
        power >>= 1;
    }

    return result;
}


}


#endif // ABEL_MATH_BINPOW_H
