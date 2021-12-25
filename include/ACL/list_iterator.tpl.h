#if !defined(ACL_LIST_ITERATOR_TPL_GUARD) || !defined(NAME_) || !defined(CONST_)
#error "DON'T use this file in your own projects!!! Use list.h instead"
#endif

#ifndef EXTRA_
#define EXTRA_
#endif

class NAME_ : public std::iterator<std::bidirectional_iterator_tag , CONST_ T> {
    using _base = std::iterator<std::bidirectional_iterator_tag , CONST_ T>;
public:
    using typename _base::iterator_category;
    using typename _base::value_type;
    using typename _base::difference_type;
    using typename _base::pointer;
    using typename _base::reference;


    constexpr NAME_() :
        lst{nullptr}, idx{BAD_IDX} {}

    constexpr NAME_(CONST_ list *lst_, CONST_ idx_t idx_) :
        lst{lst_}, idx{idx_} {

        validate();
    }

    NAME_ &operator++() {
        validate();

        idx_t nextIdx = node().next;
        REQUIRE(nextIdx != BAD_IDX);  // The list is corrupt otherwise
        idx = nextIdx;
        return *this;
    }

    NAME_ operator++(int) {
        NAME_ backup = *this;
        ++*this;
        return backup;
    }

    NAME_ &operator--() {
        validate();

        if (node().isFree()) {
            throw error("Backwards iteration isn't supported for free nodes");
        }

        idx_t prevIdx = node().prev;
        REQUIRE(prevIdx != BAD_IDX);  // The list is corrupt otherwise
        idx = prevIdx;
        return *this;
    }

    NAME_ operator--(int) {
        NAME_ backup = *this;
        --*this;
        return backup;
    }

    reference operator*() CONST_ {
        return node().getVal();
    }

    pointer operator->() CONST_ {
        return &**this;  // I know this looks weird, but trust me)
    }

    constexpr bool operator==(const NAME_ &other) const {
        return lst == other.lst && idx == other.idx;
    }

    constexpr bool operator!=(const NAME_ &other) const {
        return !(*this == other);
    }

    EXTRA_

protected:
    CONST_ list *lst;
    idx_t idx;

    friend class list;

    inline CONST_ Node &node() const {
        validate();

        return lst->buf[idx];
    }

    inline void validate() const {
        assert(lst && idx < lst->buf.getSize());
    }

};

friend class NAME_;

#undef NAME_
#undef CONST_
#undef EXTRA_
