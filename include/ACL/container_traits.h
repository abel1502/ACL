// TODO: Finish!

#ifndef ACL_CONTAINER_TRAITS_H
#define ACL_CONTAINER_TRAITS_H

#include <ACL/general.h>


namespace abel {


template <typename T, bool Specialized = true>
struct container_traits;


// Specialize this for your own containers
template <typename T>
struct container_traits<T, true> : container_traits<T, false> {};


template <typename T>
struct container_traits<T, false> {
    using value_type = typename T::value_type;
    using reference = typename T::reference;
    using const_reference = typename T::const_reference;
    using pointer = typename T::pointer;
    using const_pointer = typename T::const_pointer;
    using iterator = typename T::iterator;
    using const_iterator = typename T::const_iterator;
};


#pragma region Specializations
template <typename T, size_t N>
struct container_traits<T[N], true> {
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;
    using iterator = pointer;
    using const_iterator = const_pointer;
};
#pragma endregion Specializations


};


#endif // ACL_CONTAINER_TRAITS_H
