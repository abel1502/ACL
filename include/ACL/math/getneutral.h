#ifndef ABEL_MATH_GETNEUTRAL_H
#define ABEL_MATH_GETNEUTRAL_H

#include <ACL/general.h>
#include <ACL/type_traits.h>


namespace abel::math {


/// Overload these for your type

template <typename T>
constexpr T getMulNeutral() {
    static_assert(std::is_arithmetic_v<T>);

    return (T)1;
}

template <typename T>
constexpr T getAddNeutral() {
    static_assert(std::is_arithmetic_v<T>);

    return (T)0;
}


}


#endif // ABEL_MATH_GETNEUTRAL_H
