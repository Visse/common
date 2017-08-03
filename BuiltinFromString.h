#pragma once

#include "build_config.h"

namespace Common
{
    template< typename Type >
    COMMON_API bool BuiltinFromString( const char *str, size_t len, Type &value );
}