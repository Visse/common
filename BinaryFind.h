#pragma once

#include <algorithm>

namespace Common
{
    template< typename Iter, typename Val >
    Iter binary_find( Iter first, Iter end, Val &&val )
    {
        Iter tmp = std::lower_bound(first, end, val);
        if (tmp != end && !(val < *tmp)) {
            return tmp;
        }
        return end;
    }

    template< typename Container, typename Val >
    auto binary_find( Container &&container, Val &&val )
    {
        return binary_find(std::begin(container), std::end(container), std::forward<Val>(val));
    }
}