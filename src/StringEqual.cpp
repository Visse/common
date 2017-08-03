#include "StringEqual.h"

#include <cctype>

namespace StringUtils
{
    COMMON_API bool equal( const char *str1, size_t len1, const char *str2, size_t len2, EqualFlags flags )
    {
        if (len1 != len2) return false;

        if (all(flags, EqualFlags::IgnoreCase)) {
            for (size_t i=0; i < len1; ++i) {
                if (tolower(str1[i]) != tolower(str2[i])) return false;
            }
            return true;
        }
        else {
            return strncmp(str1, str2, len1) == 0;
        }
        
    }

    COMMON_API bool startsWith( const char *str, size_t len, const char *start, size_t startLen, EqualFlags flags )
    {
        if (len < startLen) return false;
        return equal(str, startLen, start, startLen, flags);
    }
}