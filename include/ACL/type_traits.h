#ifndef ACL_TYPE_TRAITS
#define ACL_TYPE_TRAITS

#include <ACL/general.h>
#include <type_traits>


namespace abel {


#pragma region make_(un)signed
#define CHANGE_SIGNEDNESS_(FROM, TO)                                \
    template <typename T, typename = void>                          \
    struct make_##TO {                                              \
        using type = T;                                             \
    };                                                              \
                                                                    \
    template <typename T>                                           \
    struct make_##TO<T, std::enable_if_t<std::is_##FROM##_v<T>>> {  \
        using type = std::make_##TO##_t<T>;                         \
    };                                                              \
                                                                    \
    template <typename T>                                           \
    using make_##TO##_t = typename make_##TO<T>::type;

CHANGE_SIGNEDNESS_(unsigned, signed)

CHANGE_SIGNEDNESS_(signed, unsigned)

#undef CHANGE_SIGNEDNESS_
#pragma endregion make_(un)signed


#pragma region arg_type
template <typename T, typename = void>
struct arg_type {
    using type = T &;
};

template <typename T>
struct arg_type<T, std::enable_if_t<std::is_fundamental_v<T> ||
                                    std::is_pointer_v    <T> ||
                                    std::is_reference_v  <T>>> {
    using type = T;
};

template <typename T>
using arg_type_t = typename arg_type<T>::type;
#pragma endregion arg_type


#pragma region universal_reference
template <typename B, typename T>
concept universal_ref =
    std::is_same_v<std::remove_reference_t<B>, T>;

/**
 * Usage:
 *  void f(universal_ref<MyType> auto &&arg) {
 *      volatile MyType a = std::forward<decltype(arg)>(arg);
 *  }
 *
 * Essentially, this concept allows to accept a universal reference
 * to your type in a clear and easy fashion, with comprehensible diagnostics
 */
#pragma endregion universal_reference


}


#endif // ACL_TYPE_TRAITS
