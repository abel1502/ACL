#ifndef ACL_UNIQUE_PTR_H
#define ACL_UNIQUE_PTR_H

#include <ACL/general.h>
#include <ACL/type_traits.h>
#include <ACL/cmp.h>  // TODO: Maybe should just implement the operators manually
#include <memory>  // For default_delete


namespace abel {


template <typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
public:
    DECLARE_ERROR(error, abel::error)

    using type         = T;
    using deleter_type = Deleter;
    using pointer      = T *;


    constexpr unique_ptr() noexcept :
        ptr{nullptr} {}

    constexpr unique_ptr(std::nullptr_t) noexcept :
        ptr{nullptr} {}

    explicit constexpr unique_ptr(pointer ptr_) noexcept :
        ptr{ptr_} {}

    unique_ptr(const unique_ptr &other) = delete;
    unique_ptr &operator=(const unique_ptr &other) = delete;

    constexpr unique_ptr(unique_ptr &&other) noexcept :
        ptr{other.release()} {}

    constexpr unique_ptr &operator=(unique_ptr &&other) noexcept {
        std::swap(ptr, other.ptr);

        return *this;
    }

    template <typename U, typename E>
    constexpr unique_ptr(unique_ptr<U, E> &&other) noexcept :
        ptr{other.release()} {}

    template <typename U, typename E>
    constexpr unique_ptr &operator=(unique_ptr<U, E> &&other) noexcept {
        reset(other.release());

        return *this;
    }

    constexpr unique_ptr &operator=(std::nullptr_t) {
        reset();

        return *this;
    }

    constexpr unique_ptr &operator=(pointer ptr_) {
        reset(ptr_);

        return *this;
    }

    ~unique_ptr() {
        reset();
    }

    constexpr type &operator*() const {
        if (!ptr)
            throw error("null pointer dereferencing");

        return *ptr;
    }

    constexpr pointer operator->() const noexcept {
        return ptr;
    }

    constexpr operator bool() const noexcept {
        return ptr;
    }

    constexpr pointer get() const noexcept {
        return ptr;
    }

    static constexpr deleter_type get_deleter() noexcept { return deleter_type{}; }
    // constexpr const deleter_type &get_deleter() const noexcept { return deleter_type{}; }
    // constexpr       deleter_type &get_deleter()       noexcept { return deleter_type{}; }

    constexpr pointer release() noexcept {
        pointer result = ptr;
        ptr = nullptr;
        return result;
    }

    void reset(pointer newPtr = nullptr) {
        pointer oldPtr = ptr;
        ptr = newPtr;

        assert(oldPtr != newPtr || !oldPtr);

        if (oldPtr)
            get_deleter()(oldPtr);
    }

    constexpr void swap(unique_ptr &other) noexcept {
        std::swap(ptr, other.ptr);
    }

    friend constexpr void swap(unique_ptr &a, unique_ptr &b) noexcept {
        a.swap(b);
    }

    template <typename ... As>
    void emplace(As &&... args) {
        reset(new T(std::forward<As>(args)...));
    }

    template <typename ... As>
    static unique_ptr createEmplace(As &&... args) {
        return unique_ptr(new T(std::forward<As>(args)...));
    }

    // Other comparisons are generated automatically by general.h. (More precisely, <utility>, which is included there)
    inline bool operator==(std::nullptr_t) const noexcept {
        return ptr == nullptr;
    }

    inline bool operator==(const T *other) const noexcept {
        return ptr == other;
    }

    inline bool operator<(const T *other) const noexcept {
        return ptr < other;
    }

    template <typename U, typename E>
    inline bool operator==(const unique_ptr<U, E> &other) const noexcept {
        if constexpr (!std::is_convertible_v<U *, T *> &&
                      !std::is_convertible_v<T *, U *>) {
            return false;
        }

        return ptr == other.ptr;
    }

    template <typename U, typename E>
    inline bool operator<(const unique_ptr<U, E> &other) const noexcept {
        return ptr < other.ptr;
    }

protected:
    pointer ptr;

};


// TODO: Implement
template <typename T, typename Deleter>
class unique_ptr<T[], Deleter>;


}


#endif // ACL_UNIQUE_PTR_H
