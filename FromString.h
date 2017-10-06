#pragma once

#include "BuiltinFromString.h"

namespace Common
{
#define _BUILTIN_FROM_STRING( Type )                                        \
    inline bool FromString( const char *str, size_t len, Type &value ) {    \
        return BuiltinFromString(str, len, value);                          \
    }                                                                       \
    inline bool FromString( const char *str, Type &value ) {                \
        return BuiltinFromString(str, value);                               \
    }
    _BUILTIN_FROM_STRING(int32_t);
    _BUILTIN_FROM_STRING(uint32_t);
    _BUILTIN_FROM_STRING(float);
    _BUILTIN_FROM_STRING(double);
    _BUILTIN_FROM_STRING(bool);
#undef _BUILTIN_FROM_STRING
}