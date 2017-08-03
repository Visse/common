#pragma once

#include "EnumFlagOp.h"
#include "build_config.h"

#include <cstring>

namespace StringUtils
{
    enum EqualFlags {
        None = 0,

        IgnoreCase = 1,
    };
    DECLARE_ENUM_FLAG(EqualFlags);
    
    COMMON_API bool equal( const char *str1, size_t len1, const char *str2, size_t len2, EqualFlags flags=EqualFlags::None );
    COMMON_API bool startsWith( const char *str, size_t len, const char *start, size_t startLen, EqualFlags flags=EqualFlags::None );


#define _STRING_UTILS_MAKE_VARIANTS( name, str1, len1, str2, len2 )  \
    inline bool name( const char *str1, const char *str2, EqualFlags flags=EqualFlags::None) { \
        return name(str1, std::strlen(str1), str2, std::strlen(str2), flags); \
    } \
    inline bool name( const char *str1, size_t len1, const char *str2, EqualFlags flags=EqualFlags::None ) { \
        return name(str1, len1, str2, std::strlen(str2), flags); \
    }

    _STRING_UTILS_MAKE_VARIANTS(equal, str1, len1, str2, len2);
    _STRING_UTILS_MAKE_VARIANTS(startsWith, str, strLen, start, startLen);

#undef _STRING_UTILS_MAKE_VARIANTS

}