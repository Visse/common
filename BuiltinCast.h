#include <limits>
#include <type_traits>

#include "ErrorUtils.h"

template< typename To, typename From >
To builtin_cast( From from )
{
    static_assert( std::is_fundamental<From>::value && std::is_fundamental<To>::value, "To or From isn't builtin type!");

    FATAL_ASSERT(std::numeric_limits<To>::min() <= from && from <= std::numeric_limits<To>::max(), "Value out of range!");
    return static_cast<To>(from);
}

template< typename To, typename From >
To builtin_cast_clamped( From from )
{
    static_assert( std::is_fundamental<From>::value && std::is_fundamental<To>::value, "To or From isn't builtin type!");

    if (from < std::numeric_limits<To>::min()) {
        return std::numeric_limits<To>::min();
    }
    if (from > std::numeric_limits<To>::max()) {
        return std::numeric_limits<To>::max();
    }
    
    return static_cast<To>(from);
}

