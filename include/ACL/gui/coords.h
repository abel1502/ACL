#ifndef ACL_GUI_COORDS_H
#define ACL_GUI_COORDS_H

#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <ACL/gui/rect.h>


namespace abel::gui {

using abel::math::Vector2i;
using abel::math::Vector2d;


class Coords {
public:
    constexpr Coords(const Rect<double> &screenPlane_, const Rect<double> &virtualPlane_) :
        screenPlane{screenPlane_}, virtualPlane{virtualPlane_} {}

    constexpr Coords(const Rect<double> &screenPlane_, double maxVirtX) :
        screenPlane{screenPlane_}, virtualPlane{} {

        assert(!abel::isZero(screenPlane.h()) && !abel::isZero(screenPlane.w()));

        double maxVirtY = maxVirtX * screenPlane.h() / screenPlane.w();

        virtualPlane = Rect<double>::se(-maxVirtX, -maxVirtY, maxVirtX,  maxVirtY);
    }

    constexpr Vector2d scale_v2s(Vector2d virt) const {
        if (virtualPlane.w()) {
            virt.x() *= screenPlane.w() / virtualPlane.w();
        } else {
            virt.x() = screenPlane.x0();
        }

        if (virtualPlane.h()) {
            virt.y() *= screenPlane.h() / virtualPlane.h();
        } else {
            virt.y() = screenPlane.y0();
        }

        return virt;
    }

    constexpr Vector2d scale_s2v(Vector2d screen) const {
        if (screenPlane.w()) {
            screen.x() *= virtualPlane.w() / screenPlane.w();
        } else {
            screen.x() = virtualPlane.x0();
        }

        if (screenPlane.h()) {
            screen.y() *= virtualPlane.h() / screenPlane.h();
        } else {
            screen.y() = virtualPlane.y0();
        }

        return screen;
    }

    constexpr Vector2d virt2screen(const Vector2d &virt) const {
        return screenPlane.getStart() + scale_v2s(virt - virtualPlane.getStart());
    }

    constexpr Vector2d screen2virt(const Vector2d &screen) const {
        return virtualPlane.getStart() + scale_s2v(screen - screenPlane.getStart());
    }

    constexpr Rect<double> virt2screen(const Rect<double> &virt) const {
        return Rect<double>::wh(virt2screen(virt.getPos()), scale_v2s(virt.getDiag()));
    }

    constexpr Rect<double> screen2virt(const Rect<double> &screen) const {
        return Rect<double>::wh(screen2virt(screen.getPos()), scale_s2v(screen.getDiag()));
    }

    constexpr const Rect<double> &getScreenPlane () const { return screenPlane;  }
    constexpr const Rect<double> &getVirtualPlane() const { return virtualPlane; }

protected:
    Rect<double>  screenPlane;
    Rect<double> virtualPlane;

};


}


#endif // ACL_GUI_COORDS_H

