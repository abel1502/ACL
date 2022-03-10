/**
 * This header aims to provide a basic (inefficient) alternative to std::format,
 * which is, sadly, not available in my current environment at the time of writing
 * this. ~Hopefully, this will be adapted to ACL (my favorite library. If you haven't
 * heared of it, you should definitely check it out :) )~. It is now part of ACL.
 *
 * Somewhat inspired by MSVC STL's implementation.
 */

#ifndef ACL_FORMAT_H
#define ACL_FORMAT_H

#include <ACL/general.h>
#include <ACL/type_traits.h>
#include <ACL/named_arg.h>
#include <memory>
#include <variant>
#include <string_view>
#include <string>
#include <cstring>
#include <iterator>
#include <concepts>
#include <charconv>


namespace abel::fmt {


DECLARE_ERROR(format_error, error);


enum class _format_arg_type : size_t {
    #define FMT_TYPE_(NAME, TYPE) \
        NAME,

    #include "format.dsl.h"
};


class format_parse_context {
public:
    //

protected:
    //

};


template <typename T>
class formatter {
public:
    //

protected:
    //

};


#pragma region concepts
template <typename T, typename Context>
concept has_formatter = requires(T &val, Context &ctx) {
    std::declval<typename Context::template formatter_type<std::remove_cvref_t<T>>>()
        .format(val, ctx);
};

template <typename T, typename Context>
concept has_const_formatter = has_formatter<const std::remove_reference_t<T>, Context>;
#pragma endregion concepts


#pragma region format_arg
template <typename Context>
class format_arg {
public:
    class handle {
    public:
        inline void format(format_parse_context &parse_ctx,
                           Context& format_ctx) const;

        template <typename T>
        explicit inline handle(T &&val) :
            obj{&val},
            format_func{[](format_parse_context &parse_ctx,
                           Context &format_ctx, const void *obj) {
                using value_t = std::remove_cvref_t<T>;
                typename Context::template formatter_type<value_t> formatter{};
                using qual_value_t = std::conditional_t<has_const_formatter<value_t, Context>,
                                                        const value_t, value_t>;

                parse_ctx.advance_to(formatter.parse(parse_ctx));
                format_ctx.advance_to(formatter.format(
                    *const_cast<qual_value_t *>((value_t *)obj), format_ctx
                ));

                return;
            }} {

            assert(obj);
            static_assert(has_const_formatter<T, Context> || !is_const_v<remove_reference_t<T>>);
        }

    protected:
        const void *obj;
        void (*format_func)(format_parse_context &parse_ctx, Context &format_ctx, const void *);

    };


    format_arg() noexcept {}

    template <typename T>
    explicit format_arg(const T arg_) :
        arg{arg_} {}

    explicit operator bool() const noexcept {
        return get_type() == _format_arg_type::at_none;
    }

protected:
    /// Has to be in the same order as _format_arg_type!
    std::variant<
        #define FMT_TYPE_LAST_(NAME, TYPE) \
            TYPE

        #define FMT_TYPE_(NAME, TYPE) \
            FMT_TYPE_LAST_(NAME, TYPE),

        #include "format.dsl.h"
    > arg{};


    inline _format_arg_type get_type() const {
        return (_format_arg_type)arg.index();
    }

};
#pragma endregion format_arg


#pragma region arg_helper
template <typename Context>
struct _format_arg_helper {
protected:
    template <has_formatter<Context> T>
    static auto _create_fake_inst(T &&) {
        using StrippedT = std::remove_cvref_t<T>;

        if constexpr (std::is_same_v<StrippedT, bool>) {
            return static_cast<bool>(1);
        } else if constexpr (std::is_same_v<StrippedT, char>) {
            return static_cast<char>(1);
        } else if constexpr (std::signed_integral<StrippedT> &&
                             sizeof(StrippedT) <= sizeof(int)) {
            return static_cast<int>(1);
        } else if constexpr (std::unsigned_integral<StrippedT> &&
                             sizeof(StrippedT) <= sizeof(unsigned int)) {
            return static_cast<unsigned int>(1);
        } else if constexpr (std::signed_integral<StrippedT> &&
                             sizeof(StrippedT) <= sizeof(long long)) {
            return static_cast<long long>(1);
        } else if constexpr (std::unsigned_integral<StrippedT> &&
                             sizeof(StrippedT) <= sizeof(unsigned long long)) {
            return static_cast<unsigned long long>(1);
        } else {
            // The type-erased type doesn't matter, because it is erased)
            return typename format_arg<Context>::handle{1};
        }
    }

    static auto _create_fake_inst(float) -> float;
    static auto _create_fake_inst(double) -> double;
    static auto _create_fake_inst(long double) -> long double;
    static auto _create_fake_inst(const char *) -> const char *;
    static auto _create_fake_inst(std::string_view) -> std::string_view;
    static auto _create_fake_inst(const std::string &) -> std::string_view;
    static auto _create_fake_inst(nullptr_t) -> const void *;

    template <class T> requires is_void_v<T>
    static auto _create_fake_inst(T *) -> const void *;

public:
    template <typename T>
    using storage_type = decltype(_create_fake_inst(std::declval<T>()));

    template <typename T>
    static constexpr size_t storage_size = sizeof(storage_type<std::remove_cvref_t<T>>);

};
#pragma endregion arg_helper


#pragma region arg_store
struct _format_arg_offset {
    /// Offset into the _format_arg_store::buf
    unsigned offset : (sizeof(unsigned) * 8 - 4);
    /// Arg type
    _format_arg_type type : 4;


    _format_arg_offset() noexcept = default;
    _format_arg_offset(unsigned offset_, _format_arg_type type_ = _format_arg_type::at_none) noexcept :
        offset{offset_}, type{type_} {}
};

struct _format_arg_name {
    const char *name;
    unsigned idx;
};

#pragma pack(push, 4)
static_assert(sizeof(_format_arg_name)   % 4 == 0, "Bad alignment");
static_assert(sizeof(_format_arg_offset) % 4 == 0, "Bad alignment");

template <typename Context, typename ... As>
struct _format_arg_store {
protected:
    using _helper = _format_arg_helper<Context>;

    static constexpr unsigned count = sizeof...(As);
    static constexpr size_t mem_size = (_helper::template storage_size<As> + ...);
    static constexpr unsigned count_named = named_args_count_v<As...>;


    _format_arg_name names[count_named] = {};
    _format_arg_offset offsets[count] = {};
    unsigned char buf[mem_size] = {};

    template <typename T>
    void storeErased(size_t idx, _format_arg_type type, const std::type_identity<T> &val) noexcept {
        assert(idx < count);

        const size_t offset = offsets[idx].offset;
        assert(offset + sizeof(T) <= mem_size);

        if (idx + 1 < count) {
            offsets[idx + 1].offset = offset + sizeof(T);
        }

        memcpy(buf + offset, &val, sizeof(T));
        offsets[idx].type = type;
    }

    template <typename T>
    void store(size_t idx, T &&val) noexcept {
        using ErasedT = typename _helper::template storage_type<std::remove_cvref_t<T>>;

        _format_arg_type type = _format_arg_type::at_none;

        #define FMT_TYPE_(NAME, TYPE) \
            if constexpr (std::is_same_v<ErasedT, TYPE>) { \
                type = _format_arg_type::NAME; \
            } else

        #include "format.dsl.h"
        /* else */ {
            static_assert(false, "Shouldn't be reachable");
        }

        storeErased<ErasedT>(idx, type, static_cast<ErasedT>(val));
    }

public:
    _format_arg_store(As &... args) noexcept {
        size_t idx = 0;

        (store(idx++, args), ...);
    }

};
#pragma pack(pop)


// A specialization for no arguments
template <typename Context>
struct _format_arg_store<Context> {};
#pragma endregion arg_store


#pragma region args
template <typename Context>
class format_args {
public:
    format_args() noexcept = default;

    // Specialized overload for no arguments
    format_args(const _format_arg_store<Context> &) noexcept {}

    template <typename ... As>
    format_args(const _format_arg_store<Context, As...> &store_) noexcept :
        count{sizeof...(As)}, store{store_.offsets} {}

    format_arg<Context> get(size_t i) const noexcept {
        if (i >= count) {
            return format_arg<Context>{};
        }

        const _format_arg_offset offset = *get_arg_offset_ptr((unsigned)i);
        const unsigned char *arg_place = get_arg_storage_ptr() + offset.offset;

        switch (offset.type) {
            #define FMT_TYPE_(NAME, TYPE)       \
                case _format_arg_type::NAME:    \
                    return format_arg<Context>{*reinterpret_cast<TYPE *>(arg_place)};

            #include "format.dsl.h"

        NODEFAULT
        }
    }

    format_arg<Context> get(const std::string_view &name) const noexcept {
        const _format_arg_name *arg_name = get_arg_name_ptr(0);

        // TODO: Lookup idx by name, get arg by idx
        for (unsigned i = 0; i < count_named; ++i, ++arg_name) {
            if (arg_name->name == name) {
                return get(arg_name->idx);
            }
        }

        return format_arg<Context>{};
    }

protected:
    unsigned count = 0;
    unsigned count_named = 0;
    const _format_arg_offset *store = nullptr;


    inline const _format_arg_offset *get_arg_offset_ptr(unsigned idx) const noexcept {
        assert(idx < count);
        return &store[idx];
    }

    inline const _format_arg_name *get_arg_name_ptr(unsigned idx) const noexcept {
        assert(idx < count_named);
        return &reinterpret_cast<_format_arg_name *>(store)[-(ptrdiff_t)(count_named - idx)];
    }

    inline const char *get_arg_storage_ptr() const noexcept {
        reinterpret_cast<const unsigned char*>(store + count);
    }

};
#pragma endregion args


#pragma region context
template <typename OutputIt>
requires std::output_iterator<OutputIt, char>
class format_context {
public:
    using iterator = OutputIt;
    using char_type = char;

    template <typename T>
    using formatter_type = formatter<T>;


    constexpr format_context(OutputIt it_, format_args<format_context> args_) :
        it{std::move(it_)}, args{args_} {}

    inline format_arg<format_context> arg(size_t idx) const {
        return args.get(idx);
    }

    inline format_arg<format_context> arg(const std::string_view &name) const {
        return args.get(name);
    }

    [[nodiscard]] inline iterator out() {
        return {std::move(it)};
    }

    inline void advance_to(iterator to) {
        it = std::move(to);
    }

    const format_args<format_context> &_get_args() const noexcept {
        return args;
    }

protected:
    format_args<format_context> args;
    iterator it{};

};
#pragma endregion context


#pragma region format_write
// monostate
template <typename OutputIt>
[[nodiscard]] OutputIt _format_write(OutputIt it, const std::monostate &) {
    assert(false);  // Shouldn't be called, I assume

    return it;
}

// arithmetic
template <typename OutputIt, typename T>
requires (std::is_arithmetic_v<T> && !std::is_same_v<T, char> && !std::is_same_v<T, bool>)
[[nodiscard]] OutputIt _format_write(OutputIt it, T arg) {
    // Shamelessly stolen from MSVC. Here's what occupies this mush space:
    // -DBL_MAX -> "-1.7976931348623158e+308"
    constexpr size_t BUF_SIZE = 24;

    std::array<char, BUF_SIZE> buf{};
    auto res = std::to_chars(buf.data(), buf.data() + buf.size(), arg);
    REQUIRE(res.ec == std::errc{});

    for (const char *data = buf.data(); data < res.ptr; ++data) {
        *it++ = *data;
    }

    return it;
}

// bool
template <typename OutputIt>
[[nodiscard]] OutputIt _format_write(OutputIt it, bool arg) {
    return _format_write(it, arg ? "true" : "false");
}

// char
template <typename OutputIt>
[[nodiscard]] OutputIt _format_write(OutputIt it, char arg) {
    *it++ = arg;

    return it;
}

// pointer
template <typename OutputIt>
[[nodiscard]] OutputIt _format_write(OutputIt it, const void *arg) {
    // A pointer would consume at most 16 bytes, excluding the leading "0x"
    constexpr size_t BUF_SIZE = 16;

    std::array<char, BUF_SIZE> buf{};
    auto res = std::to_chars(buf.data(), buf.data() + buf.size(), arg);
    REQUIRE(res.ec == std::errc{});

    *it++ = '0';
    *it++ = 'x';
    for (const char *data = buf.data(); data < res.ptr; ++data) {
        *it++ = *data;
    }

    return it;
}

// c string
template <typename OutputIt>
[[nodiscard]] OutputIt _format_write(OutputIt it, const char *arg) {
    REQUIRE(arg);

    for (; *arg; ++arg) {
        *it++ = *arg;
    }

    return it;
}

// string_view
template <typename OutputIt>
[[nodiscard]] OutputIt _format_write(OutputIt it, const std::string_view &arg) {
    for (const char c : arg) {
        *it++ = c;
    }

    return it;
}

// TODO: L1962 (local file)

#pragma endregion format_write


// TODO:
//    What                                      Where (in MSVC STL's <format>)
//  - _format_write(OutputIt it, * value)       (L1967-L2722)
//  - struct _format_specs                      (L1036)
//  - visit_format_arg                          (L348)
//  - the rest of the stuff                     (L2722+)
//  - encapsulate as a module instad of as a header

}


#endif // ACL_FORMAT_H
