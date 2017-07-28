#pragma once

#include <string>
#include <cstdarg>

namespace StringUtils 
{
    std::string vprintf( const char *format, std::va_list args );
    
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
}