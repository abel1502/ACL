#ifndef ACL_MATH_VECTOR_H
#define ACL_MATH_VECTOR_H

#include <ACL/general.h>
#include <ACL/math/cmath.h>
#include <ACL/type_traits.h>
#include <initializer_list>


namespace abel::math {


// Not used anywhere
// DECLARE_ERROR(vector_error, abel::error)


// Vector1
#define NAME_   Vector1
#define N_      1
#define EXTRA_                                                  \
    constexpr T &x() noexcept {                                 \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr const T &x() const noexcept {                     \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr Vector1(T new_x) noexcept :                       \
        values{new_x} {}

#include <ACL/math/vector.tpl.h>


// Vector2
#define NAME_   Vector2
#define N_      2
#define EXTRA_                                                  \
    constexpr T &x() noexcept {                                 \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr const T &x() const noexcept {                     \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr T &y() noexcept {                                 \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr const T &y() const noexcept {                     \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr Vector2(T new_x, T new_y) noexcept :              \
        values{new_x, new_y} {}                                 \
                                                                \
    /* Vector multiplication */                                 \
    constexpr T operator&(const Vector2 &other) const noexcept {\
        return x() * other.y() - y() * other.x();               \
    }                                                           \
                                                                \
    constexpr Vector2 &orthogonalize() noexcept {               \
        std::swap(x(), y());                                    \
        y() *= (T)-1;                                           \
                                                                \
        return *this;                                           \
    }                                                           \
                                                                \
    constexpr Vector2 orthogonalized() const noexcept {         \
        Vector2 result{*this};                                  \
                                                                \
        return result.orthogonalize();                          \
    }                                                           \
                                                                \
    constexpr Vector2 &rotateRadians(double radians) noexcept { \
        T new_x = (T)(cos(radians) * x() - sin(radians) * y()); \
        y() =     (T)(sin(radians) * x() + cos(radians) * y()); \
        x() = new_x;                                            \
                                                                \
        return *this;                                           \
    }                                                           \
                                                                \
    constexpr Vector2 &rotateDegrees(double degrees) noexcept { \
        rotateRadians(degrees / 180 * M_PI);                    \
                                                                \
        return *this;                                           \
    }                                                           \
                                                                \
    constexpr Vector2 rotatedRadians(double radians) const      \
                                                     noexcept { \
        Vector2 result{*this};                                  \
                                                                \
        return result.rotateRadians(radians);                   \
    }                                                           \
                                                                \
    constexpr Vector2 rotatedDegrees(double degrees) const      \
                                                     noexcept { \
        return rotatedRadians(degrees / 180 * M_PI);            \
    }                                                           \
                                                                \
    static constexpr Vector2 fromPhiRadiansR(double phiRadians, \
                                             double r)          \
                                                noexcept {      \
        return Vector2{r * std::cos(phiRadians),                \
                       r * std::sin(phiRadians)};               \
    }                                                           \
                                                                \
    static constexpr Vector2 fromPhiDegreesR(double phiDegrees, \
                                             double r)          \
                                                noexcept {      \
        return fromPhiRadiansR(phiDegrees / 180 * M_PI, r);     \
    }

#include <ACL/math/vector.tpl.h>


// Vector3
#define NAME_   Vector3
#define N_      3
#define EXTRA_                                                  \
    constexpr T &x() noexcept {                                 \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr const T &x() const noexcept {                     \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr T &y() noexcept {                                 \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr const T &y() const noexcept {                     \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr T &z() noexcept {                                 \
        return values[2];                                       \
    }                                                           \
                                                                \
    constexpr const T &z() const noexcept {                     \
        return values[2];                                       \
    }                                                           \
                                                                \
    constexpr Vector3(T new_x, T new_y, T new_z) noexcept  :    \
        values{new_x, new_y, new_z} {}                          \
                                                                \
    /* Vector multiplication */                                 \
    constexpr Vector3 operator&(const Vector3 &other) const     \
                                                      noexcept {\
        return Vector3(y() * other.z() - z() * other.y(),       \
                       z() * other.x() - x() * other.z(),       \
                       x() * other.y() - y() * other.x());      \
    }

#include <ACL/math/vector.tpl.h>


// Vector4
#define NAME_   Vector4
#define N_      4
#define EXTRA_                                                  \
    constexpr T &x() noexcept {                                 \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr const T &x() const noexcept {                     \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr T &y() noexcept {                                 \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr const T &y() const noexcept {                     \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr T &z() noexcept {                                 \
        return values[2];                                       \
    }                                                           \
                                                                \
    constexpr const T &z() const noexcept {                     \
        return values[2];                                       \
    }                                                           \
                                                                \
    constexpr T &w() noexcept {                                 \
        return values[3];                                       \
    }                                                           \
                                                                \
    constexpr const T &w() const noexcept {                     \
        return values[3];                                       \
    }                                                           \
                                                                \
    constexpr Vector4(T new_x, T new_y, T new_z, T new_w)       \
                                                    noexcept :  \
        values{new_x, new_y, new_z, new_w} {}

#include <ACL/math/vector.tpl.h>


}


#endif // ACL_MATH_VECTOR_H
