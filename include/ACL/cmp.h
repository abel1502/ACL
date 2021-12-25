/**
 *
 * This module implement as many comparison operators as in can for all classes.
 * It does so in two ways:
 *  1) It complements the existing *member* comparison operators with the ones that can be derived
 *     (including reversing direction and total ordering complementation)
 *  2) For every class that defines `__cmp__`, it uses it as a three-way comparison operator (see <=> in C++20)
 *
 */

#ifndef ACL_CMP_H
#define ACL_CMP_H

#include <ACL/general.h>
#include <ACL/template_trinkets.h>


namespace abel {


namespace _impl {
    template <typename T1, typename T2>
    using cmp_eq = decltype(std::declval<T1>().operator==(std::declval<T2>()));

    template <typename T1, typename T2>
    using cmp_ne = decltype(std::declval<T1>().operator!=(std::declval<T2>()));

    template <typename T1, typename T2>
    using cmp_lt = decltype(std::declval<T1>().operator< (std::declval<T2>()));

    template <typename T1, typename T2>
    using cmp_le = decltype(std::declval<T1>().operator<=(std::declval<T2>()));

    template <typename T1, typename T2>
    using cmp_gt = decltype(std::declval<T1>().operator> (std::declval<T2>()));

    template <typename T1, typename T2>
    using cmp_ge = decltype(std::declval<T1>().operator>=(std::declval<T2>()));

    template <typename T1, typename T2>
    using cmp_my = decltype(std::declval<T1>().__cmp__(std::declval<T2>()));

    template <typename T1, typename T2>
    constexpr bool have_my_cmp =
        is_valid_tpl_v<_impl::cmp_my, T1, T2> ||
        is_valid_tpl_v<_impl::cmp_my, T2, T1> ||
        false;

    template <typename T1, typename T2>
    constexpr bool have_any_eq =
        is_valid_tpl_v<_impl::cmp_eq, T1, T2> ||
        is_valid_tpl_v<_impl::cmp_ne, T1, T2> ||
        is_valid_tpl_v<_impl::cmp_eq, T2, T1> ||
        is_valid_tpl_v<_impl::cmp_ne, T2, T1> ||
        false;

    template <typename T1, typename T2>
    constexpr bool have_dir_lt =
        is_valid_tpl_v<_impl::cmp_lt, T1, T2> ||
        is_valid_tpl_v<_impl::cmp_gt, T2, T1> ||
        false;

    template <typename T1, typename T2>
    constexpr bool have_dir_le =
        is_valid_tpl_v<_impl::cmp_le, T1, T2> ||
        is_valid_tpl_v<_impl::cmp_ge, T2, T1> ||
        false;

    template <typename T1, typename T2>
    constexpr bool have_dir_gt =
        is_valid_tpl_v<_impl::cmp_gt, T1, T2> ||
        is_valid_tpl_v<_impl::cmp_lt, T2, T1> ||
        false;

    template <typename T1, typename T2>
    constexpr bool have_dir_ge =
        is_valid_tpl_v<_impl::cmp_ge, T1, T2> ||
        is_valid_tpl_v<_impl::cmp_le, T2, T1> ||
        false;
}


template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_any_eq<T1, T2> ||
    false, bool>
operator==(const T1 &a, const T2 &b);

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_any_eq<T1, T2> ||
    false, bool>
operator!=(const T1 &a, const T2 &b);

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_lt<T1, T2> ||
    _impl::have_dir_ge<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_le<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_gt<T1, T2> ||
    false, bool>
operator<(const T1 &a, const T2 &b);

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_gt<T1, T2> ||
    _impl::have_dir_le<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_ge<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_lt<T1, T2> ||
    false, bool>
operator>(const T1 &a, const T2 &b);

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_le<T1, T2> ||
    _impl::have_dir_gt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_lt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_ge<T1, T2> ||
    false, bool>
operator<=(const T1 &a, const T2 &b);

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_ge<T1, T2> ||
    _impl::have_dir_lt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_gt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_le<T1, T2> ||
    false, bool>
operator>=(const T1 &a, const T2 &b);


template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_any_eq<T1, T2> ||
    false, bool>
operator==(const T1 &a, const T2 &b) {
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T1, T2>) {
        return a.__cmp__(b) == 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T2, T1>) {
        return b.__cmp__(a) == 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_eq, T1, T2>) {
        return a.operator==(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ne, T1, T2>) {
        return !a.operator!=(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_eq, T2, T1>) {
        return b.operator==(a);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ne, T2, T1>) {
        return !b.operator!=(a);
    } else {
        static_assert(delay_tpl<T1>(false));
    }
}

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_any_eq<T1, T2> ||
    false, bool>
operator!=(const T1 &a, const T2 &b) {
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T1, T2>) {
        return a.__cmp__(b) != 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T2, T1>) {
        return b.__cmp__(a) != 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ne, T1, T2>) {
        return a.operator!=(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_eq, T1, T2>) {
        return !a.operator==(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ne, T2, T1>) {
        return b.operator!=(a);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_eq, T2, T1>) {
        return !b.operator==(a);
    } else {
        static_assert(delay_tpl<T1>(false));
    }
}

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_lt<T1, T2> ||
    _impl::have_dir_ge<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_le<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_gt<T1, T2> ||
    false, bool>
operator<(const T1 &a, const T2 &b) {
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T1, T2>) {
        return a.__cmp__(b) < 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T2, T1>) {
        return b.__cmp__(a) > 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_lt, T1, T2>) {
        return a.operator<(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ge, T1, T2>) {
        return !a.operator>=(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_gt, T2, T1>) {
        return b.operator>(a);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_le, T2, T1>) {
        return !b.operator<=(a);
    } else
    if constexpr (_impl::have_dir_le<T1, T2>) {
        return abel::operator!=(a, b) && abel::operator<=(a, b);
    } else
    if constexpr (_impl::have_dir_gt<T1, T2>) {
        return abel::operator!=(a, b) && !abel::operator>(a, b);
    } else {
        static_assert(delay_tpl<T1>(false));
    }
}

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_gt<T1, T2> ||
    _impl::have_dir_le<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_ge<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_lt<T1, T2> ||
    false, bool>
operator>(const T1 &a, const T2 &b) {
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T1, T2>) {
        return a.__cmp__(b) > 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T2, T1>) {
        return b.__cmp__(a) < 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_gt, T1, T2>) {
        return a.operator>(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_le, T1, T2>) {
        return !a.operator<=(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_lt, T2, T1>) {
        return b.operator<(a);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ge, T2, T1>) {
        return !b.operator>=(a);
    } else
    if constexpr (_impl::have_dir_ge<T1, T2>) {
        return abel::operator!=(a, b) && abel::operator>=(a, b);
    } else
    if constexpr (_impl::have_dir_lt<T1, T2>) {
        return abel::operator!=(a, b) && !abel::operator<(a, b);
    } else {
        static_assert(delay_tpl<T1>(false));
    }
}

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_le<T1, T2> ||
    _impl::have_dir_gt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_lt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_ge<T1, T2> ||
    false, bool>
operator<=(const T1 &a, const T2 &b) {
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T1, T2>) {
        return a.__cmp__(b) <= 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T2, T1>) {
        return b.__cmp__(a) >= 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_le, T1, T2>) {
        return a.operator<=(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_gt, T1, T2>) {
        return !a.operator>(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ge, T2, T1>) {
        return b.operator>=(a);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_lt, T2, T1>) {
        return !b.operator<(a);
    } else
    if constexpr (_impl::have_dir_lt<T1, T2>) {
        return abel::operator!=(a, b) && abel::operator<(a, b);
    } else
    if constexpr (_impl::have_dir_ge<T1, T2>) {
        return abel::operator!=(a, b) && !abel::operator>=(a, b);
    } else {
        static_assert(delay_tpl<T1>(false));
    }
}

template <typename T1, typename T2>
constexpr std::enable_if_t<
    _impl::have_my_cmp<T1, T2> ||
    _impl::have_dir_ge<T1, T2> ||
    _impl::have_dir_lt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_gt<T1, T2> ||
    _impl::have_any_eq<T1, T2> &&
    _impl::have_dir_le<T1, T2> ||
    false, bool>
operator>=(const T1 &a, const T2 &b) {
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T1, T2>) {
        return a.__cmp__(b) >= 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_my, T2, T1>) {
        return b.__cmp__(a) <= 0;
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_ge, T1, T2>) {
        return a.operator>=(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_lt, T1, T2>) {
        return !a.operator<(b);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_le, T2, T1>) {
        return b.operator<=(a);
    } else
    if constexpr (is_valid_tpl_v<_impl::cmp_gt, T2, T1>) {
        return !b.operator>(a);
    } else
    if constexpr (_impl::have_dir_gt<T1, T2>) {
        return abel::operator!=(a, b) && abel::operator>(a, b);
    } else
    if constexpr (_impl::have_dir_le<T1, T2>) {
        return abel::operator!=(a, b) && !abel::operator<=(a, b);
    } else {
        static_assert(delay_tpl<T1>(false));
    }
}


}


using abel::operator==;
using abel::operator!=;
using abel::operator< ;
using abel::operator> ;
using abel::operator<=;
using abel::operator>=;


#endif // ACL_CMP_H
