#ifndef ACL_MATH_CMATH_H
#define ACL_MATH_CMATH_H

#include <ACL/general.h>
#include <ACL/type_traits.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES 1
#endif
#include <cmath>
// std has better equivalents
#undef min
#undef max
#ifndef M_PI
#error _USE_MATH_DEFINES didn't work
#endif


namespace abel::math {


template <typename T>
T clamp(const T &val, const T &border0, const T &border1) {
    const T &from = border0 < border1 ? border0 : border1;
    const T &to   = &from == &border0 ? border1 : border0;  // To avoid an extra comparison

    if (val < from)
        return from;
    if (val > to)
        return to;
    return val;
}


}


#endif // ACL_MATH_CMATH_H
