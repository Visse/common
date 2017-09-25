#pragma once

#include "build_config.h"

namespace Common
{
    template< typename Type >
    COMMON_API bool BuiltinFromString( const char *str, size_t len, Type &value );

    template< typename Type >
    bool BuiltinFromString( const char *str, Type &value )
    {
        return BuiltinFromString(str, strlen(str), value);
    }
}