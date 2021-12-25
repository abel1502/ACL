#ifndef ACL_TEMPLATE_TRINKETS_H
#define ACL_TEMPLATE_TRINKETS_H


#include <ACL/type_traits.h>


namespace abel {


#pragma region TypeArray
namespace _impl {
    template <typename ... Ts>
    struct TypeArrayEmpty;

    template <>
    struct TypeArrayEmpty<> : public std::true_type {};

    template <typename T, typename ... Ts>
    struct TypeArrayEmpty<T, Ts...> : public std::false_type {};


    template <unsigned I, typename ... Ts>
    struct TypeArrayItem;

    template <typename T, typename ... Ts>
    struct TypeArrayItem<0, T, Ts...> {
        using type = T;
    };

    template <unsigned I, typename T, typename ... Ts>
    struct TypeArrayItem<I, T, Ts...> {
        static_assert(I > 0);  // Just in case
        using Next = TypeArrayItem<I - 1, Ts...>;

        using type = typename Next::type;
    };


    template <typename K, unsigned I, typename ... Ts>
    struct TypeArrayItemFinder;

    template <typename K, unsigned I>
    struct TypeArrayItemFinder<K, I> {
        static constexpr unsigned count = 0;
        static constexpr unsigned first_idx = -1u;
        static constexpr unsigned last_idx  = -1u;
    };

    template <typename K, unsigned I, typename ... Ts>
    struct TypeArrayItemFinder<K, I, K, Ts...> {
        using Next = TypeArrayItemFinder<K, I + 1, Ts...>;

        static constexpr unsigned count = Next::count + 1;
        static constexpr unsigned first_idx = I;
        static constexpr unsigned last_idx = count > 1 ? Next::last_idx : first_idx;
    };

    template <typename K, unsigned I, typename T, typename ... Ts>
    struct TypeArrayItemFinder<K, I, T, Ts...> {
        static_assert(!std::is_same_v<K, T>);
        using Next = TypeArrayItemFinder<K, I + 1, Ts...>;

        static constexpr unsigned count = Next::count;
        static constexpr unsigned first_idx = Next::first_idx;
        static constexpr unsigned last_idx = Next::last_idx;
    };


    template <typename ... Ts>
    struct TypeArraySizer;

    template <>
    struct TypeArraySizer<> {
        static constexpr unsigned size = 0;
    };

    template <typename T, typename ... Ts>
    struct TypeArraySizer<T, Ts...> {
        using Next = TypeArraySizer<Ts...>;

        static constexpr unsigned size = Next::size + 1;
    };

}


template <typename ... Ts>
struct TypeArray {
protected:
    template <typename T>
    using _finder = _impl::TypeArrayItemFinder<T, 0, Ts...>;

    template <unsigned I>
    using _item = _impl::TypeArrayItem<I, Ts...>;

    using _sizer = _impl::TypeArraySizer<Ts...>;

public:

    template <unsigned I>
    using type = typename _item<I>::type;

    template <unsigned I>
    using item = type<I>;  // As an alias

    static constexpr unsigned size = _sizer::size;

    static constexpr bool empty = (size == 0);

    template <typename T>
    static constexpr unsigned count = _finder<T>::count;

    template <typename T, typename = std::enable_if_t<!!count<T>>>
    static constexpr unsigned first_idx = _finder<T>::first_idx;

    template <typename T, typename = std::enable_if_t<!!count<T>>>
    static constexpr unsigned last_idx = _finder<T>::last_idx;

    template <typename T, typename = std::enable_if_t<first_idx<T> == last_idx<T>>>
    static constexpr unsigned idx = first_idx<T>;

};
#pragma endregion TypeArray


#pragma region is_valid_tpl
// https://nyorain.github.io/cpp-valid-expression.html
namespace _impl {
    template <template <typename ... > typename Tpl, typename T, typename ... As>
    struct IsValidTplHelper : public std::false_type {};

    template <template <typename ... > typename Tpl, typename ... As>
    struct IsValidTplHelper<Tpl, std::void_t<Tpl<As...>>, As...> : public std::true_type {};

}


template <template <typename ... > typename Tpl, typename ... As>
using is_valid_tpl = _impl::IsValidTplHelper<Tpl, void, As...>;

template <template <typename ... > typename Tpl, typename ... As>
constexpr bool is_valid_tpl_v = is_valid_tpl<Tpl, As...>::value;

#pragma endregion is_valid_tpl


#pragma region delay_tpl
// https://nyorain.github.io/cpp-valid-expression.html

template <typename T, typename R>
constexpr decltype(auto) delay_tpl(R &&result) {
    return std::forward<R>(result);
}
#pragma endregion delay_tpl


}


#endif // ACL_TEMPLATE_TRINKETS_H
