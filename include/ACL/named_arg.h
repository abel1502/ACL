#ifndef ACL_NAMED_ARG_H
#define ACL_NAMED_ARG_H

#include <ACL/general.h>
#include <ACL/type_traits.h>
#include <concepts>
#include <string_view>


namespace abel {


#ifdef __INTELLISENSE__
#pragma diag_suppress 864
#pragma diag_suppress 840
#endif


DECLARE_ERROR(arg_error, error)

#pragma region named_arg
template <typename T>
struct named_arg {
    const char *name;
    T value;
};

template <typename T>
using named_arg_cr = named_arg<const T &>;

template <typename T>
using named_arg_r = named_arg<T &>;

template <typename T>
using named_arg_rr = named_arg<T &&>;

template <typename T>
using named_arg_v = named_arg<T>;
#pragma endregion named_arg

#pragma region arg
template <typename T>
inline named_arg<T> arg(const char *key, T value) {
    return named_arg<T>{key, std::move(value)};
}


class _named_arg_proxy {
public:
    inline explicit _named_arg_proxy(const char *name_) noexcept :
        name{name_} {}

    template <typename T>
    inline named_arg<T> operator=(T value) const {
        return arg(name, std::move(value));
    }

protected:
    const char *name;

};

namespace arg_literals {

inline _named_arg_proxy operator ""_a(const char *name, size_t) {
    return _named_arg_proxy{name};
}

}
#pragma endregion arg

#pragma region is_named_arg
template <typename T>
struct is_named_arg : std::false_type {};

template <typename T>
struct is_named_arg<named_arg<T>> : std::true_type {};

template <typename T>
constexpr bool is_named_arg_v = is_named_arg<T>::value;


template <typename T, typename Test>
struct is_specific_named_arg : std::is_same<named_arg<T>, Test> {};

template <typename T, typename Test>
constexpr bool is_specific_named_arg_v = is_specific_named_arg<T, Test>::value;


template <typename T, typename Test>
struct is_compatible_named_arg : std::false_type {};

template <typename T, typename U>
struct is_compatible_named_arg<T, named_arg<U>> : std::is_convertible<U, T> {};

template <typename T, typename Test>
constexpr bool is_compatible_named_arg_v = is_compatible_named_arg<T, Test>::value;
#pragma endregion is_named_arg

#pragma region named_args_helper
template <typename ... As>
struct named_args_helper;

#define WRAPPERS_                                                   \
    template <typename T> requires (_first_idx<T> != -1u)           \
    static constexpr unsigned first_idx = _first_idx<T>;            \
                                                                    \
    template <typename T> requires (_last_idx<T> != -1u)            \
    static constexpr unsigned last_idx = _last_idx<T>;              \
                                                                    \
    template <typename T> requires (first_idx<T> == last_idx<T>)    \
    static constexpr unsigned only_idx = first_idx<T>;


template <>
struct named_args_helper<> {
    static constexpr unsigned count = 0;

protected:
    template <typename T>
    static constexpr unsigned _first_idx = -1u;

    template <typename T>
    static constexpr unsigned _last_idx = -1u;

public:
    WRAPPERS_

};

template <typename A, typename ... As>
struct named_args_helper<A, As...> : named_args_helper<> {
protected:
    using _next = named_args_helper<As...>;

public:
    static constexpr unsigned count = is_named_arg_v<A> + _next::count;

    template <typename T>
    static constexpr unsigned _first_idx = (std::is_same_v<A, T> ? 0 : 1 + _next::template _first_idx<T>);

    template <typename T>
    static constexpr unsigned _last_idx = (std::is_same_v<A, T> ?
        (_next::template _last_idx<T> == -1u ? 0 : _next::template _last_idx<T>) :
        _next::template _last_idx<T>);

public:
    WRAPPERS_

};

#undef WRAPPERS_

template <typename ... As>
constexpr unsigned named_args_count_v = named_args_helper<As...>::count;

#pragma endregion named_args_helper

#pragma region get_named_arg
template <typename T, bool Validate = false>
[[noreturn]] named_arg<T> &&get_arg(const std::string_view &name) {
    throw arg_error("Argument not found");
}

template <typename T, bool Validate = false, typename A, typename ... As>
named_arg<T> &&get_arg(const std::string_view &name, A &&arg, As &&... args) {
    if constexpr (is_compatible_named_arg_v<T, A>) {
        if (arg.name == name) {
            #pragma region validation
            if constexpr (Validate) {
                bool duplicates = false;

                try {
                    get_arg<T, false>(name, std::forward<As>(args)...);
                    duplicates = true;
                } catch (const arg_error &e) {}

                if (duplicates) {
                    throw arg_error("Duplicate argument");
                }
            }
            #pragma endregion validation

            return std::forward<A>(arg);
        }
    }

    return get_arg<T, Validate>(name, std::forward<As>(args)...);
}
#pragma endregion get_named_arg

#pragma region visit_named_args
template <typename Visitor, typename ... As>
concept args_visitor = requires(Visitor &&visitor, As &&... args) {
    (visitor.visit(args), ...);
};

template <typename Visitor, typename ... As>
requires args_visitor<Visitor, As...>
void visit_named_args(Visitor &&visitor, As &&... args) {
    (visitor.visit(args), ...);
}
#pragma endregion visit_named_args


#ifdef __INTELLISENSE__
#pragma diag_default 864
#pragma diag_default 840
#endif


}


#endif // ACL_NAMED_ARG_H
