#ifndef ACL_SIGNAL_H
#define ACL_SIGNAL_H

#include <ACL/general.h>
#include <ACL/vector.h>
#include <utility>
#include <functional>


namespace abel {


template <typename T>
class Signal;


template <typename ... As>
class Signal<void (As...)> {
public:
    using callback_t = void (As...);
    using func_callback_t = std::function<callback_t>;


    void invoke(As ... args) {
        REQUIRE(!beingInvoked);

        if (!callbacks && !funcCallbacks) {
            return;
        }

        beingInvoked = true;

        for (callback_t *&cb : callbacks) {
            (*cb)(args...);

            if (toUnsubCur) {
                cb = nullptr;
                toUnsubCur = false;
            }
        }

        for (func_callback_t &cb : funcCallbacks) {
            cb(args...);

            if (toUnsubCur) {
                cb = nullptr;
                toUnsubCur = false;
            }
        }

        beingInvoked = false;

        processStashedUnsub();
    }

    #define ACL_SIGNAL_TPL_GUARD
    #include <ACL/signal.tpl.h>
    #undef ACL_SIGNAL_TPL_GUARD

};


template <typename ... As>
class Signal<bool (As...)> {
public:
    using callback_t = bool (As...);
    using func_callback_t = std::function<callback_t>;


    void invoke(As ... args) {
        REQUIRE(!beingInvoked);

        if (!callbacks && !funcCallbacks)
            return;

        beingInvoked = true;

        for (callback_t *&cb : callbacks) {
            if ((*cb)(args...)) {
                toUnsubCur = true;
            }

            if (toUnsubCur) {
                cb = nullptr;
                toUnsubCur = false;
            }
        }

        for (func_callback_t &cb : funcCallbacks) {
            if (cb(args...)) {
                toUnsubCur = true;
            }

            if (toUnsubCur) {
                cb = nullptr;
                toUnsubCur = false;
            }
        }

        beingInvoked = false;

        processStashedUnsub();
    }

    #define ACL_SIGNAL_TPL_GUARD
    #include <ACL/signal.tpl.h>
    #undef ACL_SIGNAL_TPL_GUARD

};


}


#endif // ACL_SIGNAL_H
