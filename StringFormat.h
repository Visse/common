#pragma once

#include "build_config.h"

#include <string>
#include <cstdarg>

namespace StringUtils 
{
    COMMON_API std::string vprintf( const char *format, std::va_list args );
    
#ifdef __GNUC__
    __attribute__((format(printf,1,2)))
#endif
    inline std::string printf( const char *format, ... ) 
    {
        std::va_list list;
        va_start( list, format );
        std::string res = vprintf( format, list );
        va_end( list );
        return std::move(res);
    }

    COMMON_API bool vsnprintf( char *buffer, size_t bufSize, const char *format, std::va_list args );

    inline bool snprintf( char *buffer, size_t bufSize, const char *format, ... )
    {
        std::va_list list;
        va_start( list, format );
        bool res = vsnprintf( buffer, bufSize, format, list );
        va_end( list );
        return res;
    }

    template< int N >
    inline bool sprintf( char (&buffer)[N], const char *format, ... )
    {
        std::va_list list;
        va_start( list, format );
        bool res = vsnprintf( buffer, N, format, list );
        va_end( list );
        return res;
    }
}