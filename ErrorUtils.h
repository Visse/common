#pragma once

#include "build_config.h"

#if defined(__GNUC__) || defined(__GNUG__)
#   define NO_RETURN __attribute__ ((noreturn))
#   define PRINTF_FORMAT( string, args ) __attribute__ ((format (printf, string, args)));
#elif defined _MSC_VER
#   define NO_RETURN __declspec(noreturn)
#   define PRINTF_FORMAT( string, args )
#else
#   warning "Add NO_RETURN macro for this compiler."
#   define NO_RETURN
#   define PRINTF_FORMAT( string, args )
#endif


namespace internal {
    COMMON_API void NO_RETURN fatal_error( const char *format, ... ) PRINTF_FORMAT(1, 2);
    COMMON_API void error( const char *format, ... ) PRINTF_FORMAT(1, 2);
    COMMON_API void warning( const char *format, ... ) PRINTF_FORMAT(1, 2);
    COMMON_API void information( const char *format, ... ) PRINTF_FORMAT(1, 2);
    COMMON_API void performance( const char *format, ... ) PRINTF_FORMAT(1, 2);

    COMMON_API void NO_RETURN assert_failed( const char *condition, const char *format, ... ) PRINTF_FORMAT(2, 3); 
}


#ifdef IN_IDE_PARSER
    void FATAL_ERROR( const char *format, ... );
    void ERROR( const char *format, ... );
    void WARNING( const char *format, ... );
    void INFORMATION( const char *format, ... );
    void PERFORMANCE( const char *format, ... );
    void FATAL_ASSERT( bool cond, const char *format, ... );
#endif

#define LOG_MODULE(Name) static const char *_LOG__MODULE = #Name; 

#define FATAL_ERROR( ... ) ::internal::fatal_error( __VA_ARGS__ )

#define LOG_ERROR( ... ) ::internal::error( __VA_ARGS__ )
#define LOG_WARNING( ... ) ::internal::warning( __VA_ARGS__ )
#define LOG_INFORMATION( ... ) ::internal::information( __VA_ARGS__ )
#define LOG_PERFORMANCE( ... ) ::internal::performance( __VA_ARGS__ )


// The second argument must be a string literal
#define FATAL_ASSERT( cond, ... ) (void)((cond) ? 0 : ::internal::assert_failed(#cond, __VA_ARGS__),0)
