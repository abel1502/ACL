#ifndef ACL_VECTOR_H
#define ACL_VECTOR_H


#include <ACL/general.h>

#include <utility>
#include <initializer_list>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <iterator>


namespace abel {


template <typename T>
class vector {
public:
    DECLARE_ERROR(error, abel::error)

    using type = T;
    using iterator = T *;
    using const_iterator = const T *;
    using       reverse_iterator = std::reverse_iterator<      iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr unsigned DEFAULT_CAPACITY = 4;

    vector() :
        buf{nullptr},
        size{0},
        capacity{0} {

        reserve(DEFAULT_CAPACITY);
    }

    vector(std::initializer_list<T> values) :
        vector() {

        reserve((unsigned)values.size());
        extend(values.begin(), values.end());
    }

    template <typename InputIt>
    vector(InputIt &&begin_, InputIt &&end_) :
        vector() {

        extend(std::forward<InputIt>(begin_), std::forward<InputIt>(end_));
    }

    vector(unsigned new_size) :
        vector() {

        reserve(new_size);
        for (unsigned i = 0; i < new_size; ++i) {
            append();
        }
    }

    vector(unsigned new_size, const T &value) :
        vector() {

        reserve(new_size);
        for (unsigned i = 0; i < new_size; ++i) {
            append(value);
        }
    }

    vector(const vector &other) :
        vector() {

        reserve(other.capacity);
        for (unsigned i =0; i < other.size; ++i) {
            append(other[i]);
        }
    }

    vector &operator=(const vector &other) {
        destroy();

        reserve(other.capacity);

        for (unsigned i =0; i < other.size; ++i) {
            append(other[i]);
        }

        return *this;
    }

    vector(vector &&other) noexcept {
        std::swap(buf, other.buf);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);
    }

    vector &operator=(vector &&other) noexcept {
        std::swap(buf, other.buf);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);

        return *this;
    }

    ~vector() noexcept {
        destroy();
    }

    T &operator[](int idx) {
        if ((int)-size > idx || idx >= (int)size)
            throw error("Vector subscript by illegal index");

        idx = (idx + size) % size;

        return buf[idx];
    }

    const T &operator[](int idx) const {
        return const_cast<vector *>(this)->operator[](idx);
    }

    /// The following methods return whether the buffer was moved

    #define DECLARE_APPEND_BODY_(...) {                 \
        bool moved = false;                             \
                                                        \
        if (size >= capacity) {                         \
            moved = reserve(capacity * 2);              \
                                                        \
            assert(capacity > size);                    \
        }                                               \
                                                        \
        T *newSpot = new (&buf[size++]) T(__VA_ARGS__); \
        assert(newSpot == &buf[size - 1]);              \
                                                        \
        return moved;                                   \
    }

    bool append() {
        DECLARE_APPEND_BODY_()
    }

    bool append(const T &value) {
        DECLARE_APPEND_BODY_(value)
    }

    bool append(T &&value) {
        DECLARE_APPEND_BODY_(std::move(value))
    }

    template <typename ... Ts>
    bool appendEmplace(Ts &&... args) {
        DECLARE_APPEND_BODY_(std::forward<Ts>(args)...)
    }

    #undef DECLARE_APPEND_BODY_

    #define DECLARE_EXTEND_BODY_(FOR_CODE, APPEND_CALL) {       \
        bool moved = false;                                     \
                                                                \
        for (FOR_CODE) {                                        \
            /* Intentionally not boolean or, because we   */    \
            /* don't want shortcutting to take place here */    \
            moved |= !!(APPEND_CALL);                           \
        }                                                       \
                                                                \
        return moved;                                           \
    }

    bool extend(unsigned count) {
        DECLARE_EXTEND_BODY_(unsigned i = 0; i < count; ++i, appendEmplace())
    }

    bool extend(std::initializer_list<T> values) {
        DECLARE_EXTEND_BODY_(const T &val : values, append(val))
    }

    template <typename InputIt>
    bool extend(InputIt begin_, InputIt end_) {
        DECLARE_EXTEND_BODY_(; begin_ != end_; ++begin_, append(*begin_))
    }

    #undef DECLARE_EXTEND_BODY_

    T popVal() {
        if (isEmpty())
            throw error("Pop from empty vector");

        assert(buf);

        T result = std::move(buf[--size]);

        buf[size].~T();

        return result;
    }

    void pop() {
        if (isEmpty())
            throw error("Pop from empty vector");

        assert(buf);

        buf[--size].~T();
    }

    void swapPop(int idx) {
        std::swap((*this)[idx], (*this)[-1]);
        pop();
    }

    T swapPopVal(int idx) {
        std::swap((*this)[idx], (*this)[-1]);
        return popVal();
    }

    constexpr bool isEmpty() const noexcept {
        return size == 0;
    }

    constexpr operator bool() const noexcept {
        return !isEmpty();
    }

    constexpr unsigned getSize() const noexcept {
        return size;
    }

    constexpr unsigned getCapacity() const noexcept {
        return capacity;
    }

    /// Warning: Don't save the pointer!
    const T *getBuf() const noexcept {
        return buf;
    }

    /// Warning: Don't save the pointer!
    T *getBuf() noexcept {
        return buf;
    }

    bool testWithin(const T *item) const {
        // One check is sufficient because size_t is signed
        return (size_t)(item - buf) < size && ((size_t)item - (size_t)buf) % sizeof(T) == 0;
    }

    unsigned ptrToIdx(const T *item) const {
        if (!testWithin(item))
            throw error("Item doesn't belong to the vector");

        return (unsigned)(item - buf);
    }

    inline unsigned refToIdx(const T &ref) const { return ptrToIdx(&ref); }

    constexpr const_iterator begin() const noexcept {
        return buf;
    }

    constexpr iterator begin() noexcept {
        return buf;
    }

    constexpr const_iterator end() const noexcept {
        return buf + size;
    }

    constexpr iterator end() noexcept {
        return buf + size;
    }

    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end  ()}; }
    constexpr       reverse_iterator rbegin()       noexcept { return       reverse_iterator{end  ()}; }
    constexpr const_reverse_iterator rend  () const noexcept { return const_reverse_iterator{begin()}; }
    constexpr       reverse_iterator rend  ()       noexcept { return       reverse_iterator{begin()}; }

    /// Returns whether the buffer was moved
    bool reserve(unsigned new_capacity) {
        if (new_capacity == 0)
            new_capacity = DEFAULT_CAPACITY;

        assert(size <= capacity);
        if (new_capacity <= capacity)
            return false;

        T *newBuf = (T *)calloc(new_capacity, sizeof(T));

        if (!newBuf) {
            throw error("Buffer allocation failed");
        }

        if (buf) {
            for (unsigned i = 0; i < size; ++i) {
                new (&newBuf[i]) T(std::move(buf[i]));
                buf[i].~T();
            }

            free(buf);
        }

        buf = newBuf;
        capacity = new_capacity;

        return true;
    }

    void clear() {
        destroy();
        reserve(DEFAULT_CAPACITY);
    }

    constexpr void swap(vector &other) noexcept {
        std::swap(buf, other.buf);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);
    }

    friend constexpr void swap(vector &a, vector &b) noexcept {
        a.swap(b);
    }

    /// Keep only the elements that match the predicate. Maintains the order of the ones remaining
    template <typename P>
    constexpr void filter(P predicate) {
        unsigned newIdx = 0;

        for (unsigned i = 0; i < size; ++i) {
            if (!predicate(buf[i])) {
                buf[i].~T();
                continue;
            }

            if (newIdx < i) {
                T *addr = new (&buf[newIdx]) T(std::move(buf[i]));
                assert(addr == &buf[newIdx]);
            }

            newIdx++;
        }

        resize(newIdx);
    }

    /// Same as filter, at the cost of possibly messing up the order, this one performs less moves on T
    /// If T's movement is expensive, this will probably be faster.
    template <typename P>
    constexpr void filterUnordered(P predicate) {
        for (unsigned i = 0; i < size; /* no increment intentionally */) {
            if (predicate(buf[i])) {
                i++;
                continue;
            }

            swapPop(i);
        }
    }

    bool resize(unsigned new_size) {
        if (new_size > size) {
            return extend(new_size - size);
        }

        while (new_size < size) {
            pop();
        }

        return false;
    }

protected:
    T *buf;
    unsigned size;
    unsigned capacity;

    void destroy() noexcept {
        if (buf) {
            for (unsigned i = 0; i < size; ++i) {
                buf[i].~T();
            }

            free(buf);
        }

        buf = nullptr;
        size = 0;
        capacity = 0;
    }

};


}


#include <utility>


#endif // ACL_VECTOR_H
