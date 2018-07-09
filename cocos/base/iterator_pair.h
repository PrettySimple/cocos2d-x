//
//  iterator_pair.h
//  libcocos2d
//
//  Created by Mathieu Garaud on 06/02/2018.
//

#ifndef CC_BASE_ITERATORPAIR_H
#define CC_BASE_ITERATORPAIR_H

#include "platform/CCPlatformMacros.h"
#include <type_traits>
#include <utility>

NS_CC_BEGIN

template <typename T>
struct remove_cvref
{
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename PairIter>
class iterator_pair
{
public:
    using first_type_t = typename remove_cvref_t<PairIter>::first_type;
    using second_type_t = typename remove_cvref_t<PairIter>::second_type;

    inline explicit iterator_pair(PairIter&& p)
    : _p(std::forward<PairIter>(p))
    {
    }
    inline first_type_t begin() { return _p.first; }
    inline second_type_t end() { return _p.second; }
    inline first_type_t begin() const noexcept { return _p.first; }
    inline second_type_t end() const noexcept { return _p.second; }

private:
    PairIter _p;
};

template <typename PairIter>
inline iterator_pair<PairIter> make_iterator_pair(PairIter&& p)
{
    return iterator_pair<PairIter>(std::forward<PairIter>(p));
}

NS_CC_END

#endif // CC_BASE_ITERATORPAIR_H
