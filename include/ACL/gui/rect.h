#ifndef ACL_GUI_RECT_H
#define ACL_GUI_RECT_H

#include <ACL/math/vector.h>
#include <ACL/type_traits.h>
#include <ACL/math/cmath.h>


namespace abel::gui {


template <typename T>
class Rect {
    //static_assert(std::is_arithmetic_v<T>);
public:
    using type = T;
    using signed_type = typename abel::make_signed_t<T>;
    using Vector2T = typename abel::math::Vector2<signed_type>;


    constexpr Rect() noexcept {}

    /// Width-height
    static constexpr Rect wh(T x, T y, T w, T h) noexcept {
        return Rect(x, y, w, h);
    }

    /// Start-end
    template <bool SORT = true>
    static constexpr Rect se(T x0, T y0, T x1, T y1) noexcept {
        if constexpr (SORT) {
            if (x0 > x1)
                std::swap(x0, x1);
            if (y0 > y1)
                std::swap(y0, y1);
        }

        return Rect(x0, y0, x1, y1, 0);
    }

    /// Width-height, vector
    static constexpr Rect wh(const Vector2T &from, const Vector2T &size) noexcept {
        return Rect::wh(from.x(), from.y(), size.x(), size.y());
    }

    /// Start-end, vector
    template <bool SORT = true>
    static constexpr Rect se(const Vector2T &start, const Vector2T &end) noexcept {
        return Rect::se<SORT>(start.x(), start.y(), end.x(), end.y());
    }

    constexpr T  x() const noexcept { return x_; }
    constexpr T  y() const noexcept { return y_; }
    constexpr T  w() const noexcept { return w_; }
    constexpr T  h() const noexcept { return h_; }
    constexpr T x0() const noexcept { return x_; }
    constexpr T y0() const noexcept { return y_; }
    constexpr T x1() const noexcept { return x_ + w_; }
    constexpr T y1() const noexcept { return y_ + h_; }

    constexpr void  x(T val) noexcept { x_ = val; }
    constexpr void  y(T val) noexcept { y_ = val; }
    constexpr void  w(T val) noexcept { w_ = val; }
    constexpr void  h(T val) noexcept { h_ = val; }
    constexpr void x0(T val) noexcept { w_ += x_ - val; x_ = val; }
    constexpr void y0(T val) noexcept { h_ += y_ - val; y_ = val; }
    constexpr void x1(T val) noexcept { w_ = val - x_; }
    constexpr void y1(T val) noexcept { h_ = val - y_; }

    // Pos and Start differ!
    // Pos refers to x and y of wh, whereas Start refers to x0 and y0 of se
    // Thus, setting them maintains either Diag or End, respectively

    constexpr Vector2T getStart() const noexcept { return Vector2T(x0(), y0()); }
    constexpr Vector2T getEnd  () const noexcept { return Vector2T(x1(), y1()); }
    constexpr Vector2T getPos  () const noexcept { return Vector2T(x (), y ()); }
    constexpr Vector2T getDiag () const noexcept { return Vector2T(w (), h ()); }

    constexpr void setStart(const Vector2T &value) noexcept { x0(value.x()); y0(value.y()); }
    constexpr void setEnd  (const Vector2T &value) noexcept { x1(value.x()); y1(value.y()); }
    constexpr void setPos  (const Vector2T &value) noexcept { x (value.x()); y (value.y()); }
    constexpr void setDiag (const Vector2T &value) noexcept { w (value.x()); h (value.y()); }

    constexpr Vector2T getCenter() const noexcept { return (getStart() + getEnd()) / 2; }
    constexpr void setCenter(const Vector2T &value) noexcept { setPos(value - getDiag() / 2); }


    constexpr Vector2T getVertex(bool xCoord, bool yCoord) const {
        return Vector2T(x() + (!!xCoord) * w(),
                        y() + (!!yCoord) * h());
    }

    template <typename T2>
    constexpr operator Rect<T2>() const {
        return Rect<T2>((T2)x(), (T2)y(), (T2)w(), (T2)h());
    }

    Rect &clamp(const Rect &bounds) noexcept {
        x0(abel::math::clamp(x0(), bounds.x0(), bounds.x1()));
        x1(abel::math::clamp(x1(), bounds.x0(), bounds.x1()));
        y0(abel::math::clamp(y0(), bounds.y0(), bounds.y1()));
        y1(abel::math::clamp(y1(), bounds.y0(), bounds.y1()));

        return *this;
    }

    Rect clamped(const Rect &bounds) const noexcept {
        Rect result{*this};

        return result.clamp(bounds);
    }

    constexpr bool contains(const Vector2T &point) const noexcept {
        return x0() <= point.x() && point.x() <= x1() &&
               y0() <= point.y() && point.y() <= y1();
    }

    constexpr bool contains(const Rect &other) const noexcept {
        return contains(other.getStart()) && contains(other.getEnd());
    }

    constexpr Rect relRect(const Rect &relative, bool allowOverflow = false) const noexcept {
        Rect result = Rect::wh(getPos() + relative.getPos(), relative.getDiag());

        if (!allowOverflow) {
            result.clamp(*this);
        }

        return result;
    }

    constexpr Rect relVec(const Vector2T &relative, bool allowOverflow = false) const noexcept {
        Vector2T result = getPos() + relative;

        if (!allowOverflow) {
            result.clamp(getDiag());
        }

        return result;
    }

    constexpr Rect &pad(signed_type left, signed_type right, signed_type top, signed_type bottom) noexcept {
        x0(x0() + left);
        x1(x1() - right);
        y0(y0() + top);
        y1(y1() - bottom);

        return *this;
    }

    constexpr Rect &pad(signed_type horizontal, signed_type vertical) noexcept {
        return pad(horizontal, horizontal, vertical, vertical);
    }

    constexpr Rect &pad(signed_type amount) noexcept {
        return pad(amount, amount);
    }

    constexpr Rect &padH(signed_type amount) noexcept {
        return pad(amount, 0);
    }

    constexpr Rect &padV(signed_type amount) noexcept {
        return pad(0, amount);
    }

    constexpr Rect padded(signed_type left, signed_type right, signed_type top, signed_type bottom) const noexcept {
        Rect result{*this};

        return result.pad(left, right, top, bottom);
    }

    constexpr Rect padded(signed_type horizontal, signed_type vertiacal) const noexcept {
        Rect result{*this};

        return result.pad(horizontal, vertiacal);
    }

    constexpr Rect padded(signed_type amount) const noexcept {
        Rect result{*this};

        return result.pad(amount);
    }

    constexpr Rect paddedH(signed_type amount) const noexcept {
        Rect result{*this};

        return result.padH(amount);
    }

    constexpr Rect paddedV(signed_type amount) const noexcept {
        Rect result{*this};

        return result.padV(amount);
    }

    constexpr Rect &operator+=(const Vector2T &delta) noexcept {
        setPos(getPos() + delta);

        return *this;
    }

    constexpr Rect &operator-=(const Vector2T &delta) noexcept {
        setPos(getPos() - delta);

        return *this;
    }

    constexpr Rect operator+(const Vector2T &delta) const noexcept {
        Rect result{*this};

        return result += delta;
    }

    constexpr Rect operator-(const Vector2T &delta) const noexcept {
        Rect result{*this};

        return result -= delta;
    }

    constexpr bool isEmpty() const noexcept {
        return x0() >= x1() || y0() >= y1();
    }

    constexpr Rect &normalizeEmpty() noexcept {
        if (x0() > x1())
            x1(x0());
        if (y0() > y1())
            y1(y0());

        return *this;
    }

    constexpr Rect normalizedEmpty() const noexcept {
        Rect result{*this};

        return result.normalizeEmpty();
    }

protected:
    T x_{},
      y_{},
      w_{},
      h_{};


    /// Width-height
    constexpr Rect(T new_x, T new_y, T new_w, T new_h) noexcept {
        x(new_x);
        y(new_y);
        w(new_w);
        h(new_h);
    }

    /// Start-end
    constexpr Rect<T>(T new_x0, T new_y0, T new_x1, T new_y1, int) noexcept {
        x0(new_x0);
        y0(new_y0);
        x1(new_x1);
        y1(new_y1);
    }

};


}


#endif // ACL_GUI_RECT_H
