#ifndef ACL_SIGNAL_TPL_GUARD
#error "DON'T use this file in your own projects!!! Use signal.h instead"
#endif


public:
    Signal() {}

    inline void operator()(As ... args) {
        invoke(args...);
    }

    void subscribe(callback_t *cb) {
        // Subscribing during invocation cannot guarantee whether the new callback will be called this time or not
        REQUIRE(!beingInvoked);

        callbacks.append(cb);
    }

    void subscribe(const func_callback_t &cb) {
        // Same as above
        REQUIRE(!beingInvoked);

        funcCallbacks.append(cb);
    }

    void subscribe(func_callback_t &&cb) {
        // Same as above
        REQUIRE(!beingInvoked);

        funcCallbacks.append(std::move(cb));
    }

    inline Signal &operator+=(callback_t *cb) {
        subscribe(cb);

        return *this;
    }

    inline Signal &operator+=(const func_callback_t &cb) {
        subscribe(cb);

        return *this;
    }

    inline Signal &operator+=(func_callback_t &&cb) {
        subscribe(std::move(cb));

        return *this;
    }

    template <typename T>
    inline Signal &operator+=(T &&obj) {
        return *this += func_callback_t(std::forward<T>(obj));
    }

    void unsubscribe(callback_t *cb) {
        if (beingInvoked) {
            queueUnsubscribe(cb);
            return;
        }

        unsigned i = findCallback(cb);
        if (i == BAD_IDX)
            return;

        callbacks.swapPop(i);
    }

    void unsubscribeAll(callback_t *cb) {
        if (beingInvoked) {
            queueUnsubscribeAll(cb);
            return;
        }

        callbacks.filter([cb](callback_t *curCb){ return curCb != cb; });
    }

    void unsubscribeCur() {
        REQUIRE(beingInvoked);

        toUnsubCur = true;
    }

    inline Signal &operator-=(callback_t *cb) {
        unsubscribe(cb);

        return *this;
    }

    void clear() {
        if (beingInvoked) {
            toClear = true;
            return;
        }

        callbacks.clear();
        funcCallbacks.clear();
        toClear = false;
    }

    bool hasCallback(callback_t *cb) const {
        return findCallback(cb) != BAD_IDX;
    }

    inline bool isBeingInvoked() const {
        return beingInvoked;
    }

protected:
    static constexpr unsigned BAD_IDX = -1u;
    struct UnsubRequest {
        callback_t *cb;
        bool all;

        constexpr UnsubRequest(callback_t *cb_ = nullptr, bool all_ = false) :
            cb{cb_}, all{all_} {};
    };

    // I initially wanted to make this a set, but the more I think about it,
    // the more it seems like a vector is the way to go. It will at least
    // allow subscribing a callback more than once. Although I think I should
    vector<callback_t *> callbacks{};
    vector<func_callback_t> funcCallbacks{};
    vector<UnsubRequest> unsubQueue{};
    mutable bool beingInvoked = false;  // Doesn't have to be mutable anymore, but it still makes sense this way
    bool toUnsubCur = false;
    bool toClear = false;


    // Returns callback index, or BAD_IDX on error
    unsigned findCallback(callback_t *cb) const {
        for (unsigned i = 0; i < callbacks.getSize(); ++i) {
            if (callbacks[i] == cb) {
                return i;
            }
        }

        return BAD_IDX;
    }

    void queueUnsubscribe(callback_t *cb) {
        unsubQueue.appendEmplace(cb, false);
    }

    void queueUnsubscribeAll(callback_t *cb) {
        unsubQueue.appendEmplace(cb, true);
    }

    void processStashedUnsub() {
        REQUIRE(!beingInvoked);

        if (toClear) {
            unsubQueue.clear();
            clear();
            return;
        }

        for (const UnsubRequest &req : unsubQueue) {
            if (req.all) {
                unsubscribeAll(req.cb);
            } else {
                unsubscribe(req.cb);
            }
        }

        unsubQueue.clear();

        unsubscribeNull();
    }

    void unsubscribeNull() {
            callbacks.filter         ([](           callback_t *cb){ return cb; });
        funcCallbacks.filterUnordered([](const func_callback_t &cb){ return cb; });
    }
