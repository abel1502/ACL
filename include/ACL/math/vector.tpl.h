#if !defined(NAME_) || !defined(N_)
#error "DON'T use this file in your own projects!!! Use vector.h instead"
#endif

#if !defined(EXTRA_)
#define EXTRA_
#endif

template <typename T>
class NAME_ {
    static_assert(1 <= N_ && N_ <= 4);
    //static_assert(std::is_signed_v<T>);  // May theoretically be removed, because constexpr would still work

public:
    using type = T;
    using arg_type = abel::arg_type_t<T>;
    using const_arg_type = abel::arg_type_t<const T>;
    static constexpr unsigned DIM = N_;

    T values[DIM];

    constexpr NAME_() noexcept :
        values{} {}

    constexpr NAME_(const NAME_ &other) noexcept = default;

    // Because for N == 1 this is equivalent to the complete constructor
    #if N_ > 1
    explicit constexpr NAME_(const_arg_type val) noexcept :
        values{} {

        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = val;
        }
    }
    #endif

    constexpr NAME_ &operator=(const NAME_ &other) noexcept = default;

    constexpr NAME_ &operator+=(const NAME_ &other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] += other.values[i];
        }

        return *this;
    }

    constexpr NAME_ &operator-=(const NAME_ &other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] -= other.values[i];
        }

        return *this;
    }

    constexpr NAME_ &operator*=(const_arg_type other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] *= other;
        }

        return *this;
    }

    constexpr NAME_ &operator/=(const_arg_type other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] /= other;
        }

        return *this;
    }

    /// Scalar multiplication
    constexpr T operator*(const NAME_ &other) const noexcept {
        T result{};

        for (unsigned i = 0; i < DIM; ++i) {
            result += values[i] * other.values[i];
        }

        return result;
    }

    constexpr NAME_ &normalize() noexcept {
        double length_ = length();

        if (length_ == 0.) {
            DBG("Zero-vector normalization. Ignoring");
            return *this;
        }

        return *this *= 1 / length_;
    }

    constexpr NAME_ normalized() const noexcept {
        NAME_ result{*this};

        return result.normalize();
    }

    constexpr NAME_ &mirrorAgainst(const NAME_ &against) noexcept {
        return *this = mirroredAgainst(against);
    }

    constexpr NAME_ mirroredAgainst(const NAME_ &against) const noexcept {
        return against * (against * *this) * 2 / against.magnitude() - *this;
    }

    constexpr NAME_ &mirrorAlong(const NAME_ &along) noexcept {
        return *this = mirroredAlong(along);
    }

    constexpr NAME_ mirroredAlong(const NAME_ &along) const noexcept {
        return *this - projected(along) * 2;
    }

    constexpr NAME_ &project(const NAME_ &onto) noexcept {
        return *this = projected(onto);
    }

    constexpr NAME_ projected(const NAME_ &onto) const noexcept {
        return onto * (*this * onto) / onto.magnitude();
    }

    constexpr NAME_ &clamp(const NAME_ &from, const NAME_ &to) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = math::clamp(values[i], from.values[i], to.values[i]);
        }

        return *this;
    }

    constexpr NAME_ clamped(const NAME_ &from, const NAME_ &to) const noexcept {
        NAME_ result{*this};

        return result.clamp(from, to);
    }

    constexpr double length() const noexcept {
        if constexpr (DIM == 1)
            return (double)values[0];
        if constexpr (DIM == 2)
            return std::hypot((double)values[0], (double)values[1]);
        if constexpr (DIM == 3)
            return std::hypot((double)values[0], (double)values[1], (double)values[2]);
        return std::sqrt((double)magnitude());
    }

    constexpr T magnitude() const noexcept {
        T result{};

        for (unsigned i = 0; i < DIM; ++i)
            result += values[i] * values[i];

        return result;
    }

    constexpr bool isZero() const noexcept {
        for (unsigned i = 0; i < DIM; ++i)
            if (abel::sgnDbl(values[i]))
                return false;

        return true;
    }

    constexpr friend NAME_ operator+(NAME_ self, const NAME_ &other) noexcept {
        return self += other;
    }

    constexpr friend NAME_ operator-(NAME_ self, const NAME_ &other) noexcept {
        return self -= other;
    }

    constexpr friend NAME_ operator*(NAME_ self, const_arg_type other) noexcept {
        return self *= other;
    }

    constexpr friend NAME_ operator/(NAME_ self, const_arg_type other) noexcept {
        return self /= other;
    }

    constexpr friend NAME_ operator*(const_arg_type other, NAME_ self) noexcept {
        return self *= other;
    }

    constexpr friend NAME_ operator-(NAME_ self) noexcept {
        return self *= -1.;
    }

    constexpr bool operator==(const NAME_ &other) const noexcept {
        for (unsigned i = 0; i < DIM; ++i)
            if (abel::cmpDbl(values[i], other.values[i]))
                return false;

        return true;
    }

    constexpr bool operator!=(const NAME_ &other) const noexcept {
        return !operator==(other);
    }

    template <typename U>
    constexpr explicit operator NAME_<U>() const noexcept {
        NAME_<U> result{};

        for (unsigned i = 0; i < DIM; ++i)
            result.values[i] = (U)values[i];

        return result;
    }

    EXTRA_

};


#define CONCAT_INNER_(A, B)  A##B
#define CONCAT_(A, B)  CONCAT_INNER_(A, B)

using CONCAT_(NAME_, i) = NAME_<int>;
using CONCAT_(NAME_, f) = NAME_<float>;
using CONCAT_(NAME_, d) = NAME_<double>;

#undef CONCAT_INNER_
#undef CONCAT_

#undef NAME_
#undef N_
#undef EXTRA_
