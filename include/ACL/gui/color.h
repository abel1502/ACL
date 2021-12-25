#ifndef ACL_GUI_COLOR_H
#define ACL_GUI_COLOR_H

#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <cstdint>


namespace abel::gui {


struct PackedColor {
    constexpr PackedColor() :
        PackedColor(0, 0, 0) {}

    constexpr PackedColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0) :
        B{b}, G{g}, R{r}, A{a} {}

    union {
        struct {
            unsigned char B, G, R, A;
        };

        uint32_t value;
    };
};
static_assert(sizeof(PackedColor) == 4);
static_assert(offsetof(PackedColor, B) == 0);
static_assert(offsetof(PackedColor, G) == 1);
static_assert(offsetof(PackedColor, R) == 2);
static_assert(offsetof(PackedColor, A) == 3);


class Color : public abel::math::Vector3f {
public:
    static const Color BLACK, WHITE, RED, ORANGE, YELLOW, GREEN, LBLUE, BLUE, PURPLE;


    constexpr Color() noexcept :
        abel::math::Vector3f() {}

    constexpr Color(float brightness) noexcept :
        abel::math::Vector3f(brightness) {}

    constexpr Color(float new_r, float new_g, float new_b) noexcept :
        abel::math::Vector3f(new_r, new_g, new_b) {}

    constexpr Color(const PackedColor &packed) noexcept :
        Color((float)packed.R / 255.f,
              (float)packed.G / 255.f,
              (float)packed.B / 255.f) {}

    template <typename T>
    constexpr Color(const abel::math::Vector3<T> &other) noexcept :
        abel::math::Vector3f(other) {}

    template <typename T>
    constexpr Color &operator=(const abel::math::Vector3<T> &other) noexcept {
        return abel::math::Vector3f::operator=(other);
    }

    static constexpr Color fromRGB(unsigned char r, unsigned char g, unsigned char b) {
        return abel::math::Vector3f((float)r, (float)g, (float)b) / 255.f;
    }

    constexpr float &r() noexcept {
        return values[0];
    }

    constexpr const float &r() const noexcept {
        return values[0];
    }

    constexpr float &g() noexcept {
        return values[1];
    }

    constexpr const float &g() const noexcept {
        return values[1];
    }

    constexpr float &b() noexcept {
        return values[2];
    }

    constexpr const float &b() const noexcept {
        return values[2];
    }

    constexpr Color &clamp() noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            if (values[i] < 0.f)
                values[i] = 0.f;

            if (values[i] > 1.f)
                values[i] = 1.f;
        }

        return *this;
    }

    constexpr Color clamped() const noexcept {
        Color result{*this};

        return result.clamp();
    }

    constexpr PackedColor pack(const_arg_type alpha = 1) const noexcept {
        Color tmp = clamped() * 255;

        return PackedColor{(unsigned char)tmp.r(),
                           (unsigned char)tmp.g(),
                           (unsigned char)tmp.b(),
                           (unsigned char)(alpha * 255)};
    }

    constexpr Color &reflect(const Color &other) noexcept {
        r() *= other.r();
        g() *= other.g();
        b() *= other.b();

        return *this;
    }

    constexpr Color reflected(const Color &other) const noexcept {
        Color result{*this};

        return result.reflect(other);
    }

};


inline constexpr Color Color::BLACK  {0., 0., 0.};
inline constexpr Color Color::WHITE  {1., 1., 1.};
inline constexpr Color Color::RED    {1., 0., 0.};
inline constexpr Color Color::ORANGE {1., .5, 0.};
inline constexpr Color Color::YELLOW {1., 1., 0.};
inline constexpr Color Color::GREEN  {0., 1., 0.};
inline constexpr Color Color::LBLUE  {0., 1., 1.};
inline constexpr Color Color::BLUE   {0., 0., 1.};
inline constexpr Color Color::PURPLE {1., 0., 1.};


}


#endif // ACL_GUI_COLOR_H

