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
    enum LogLevel : int {
        debug = -1,
        information = 0,
        performance = 1,
        warning = 2,
        error = 3,
        fatal = 4,
    };

    COMMON_API void NO_RETURN fatal_error( const char *module, const char *file, int line, const char *format, ... ) PRINTF_FORMAT(4, 5);
    COMMON_API void log( int level, const char *module, const char *file, int line, const char *format, ... ) PRINTF_FORMAT(5, 6);
    
    COMMON_API void NO_RETURN assert_failed( const char *module, const char *file, int line, const char *condition, const char *format, ... ) PRINTF_FORMAT(5, 6);
    COMMON_API void NO_RETURN assert_failed( const char *module, const char *file, int line, const char *condition );

    static const struct LogModuleTag {} _log_module_tag;
    static inline const char* _get_log_module( const LogModuleTag& ) {
        return "Global";
    }
}


#ifdef IN_IDE_PARSER
    void FATAL_ERROR( const char *format, ... );
    void ERROR( const char *format, ... );
    void WARNING( const char *format, ... );
    void INFORMATION( const char *format, ... );
    void PERFORMANCE( const char *format, ... );
    void FATAL_ASSERT( bool cond, const char *format, ... );
#endif

#define CREATE_LOG_MODULE(Name)                                             \
    static const struct {                                                   \
        const char* operator () ( const ::internal::LogModuleTag& ) const { \
            return Name;                                                    \
        }                                                                   \
    } _get_log_module;

#define FATAL_ERROR( ... ) ::internal::fatal_error( _get_log_module(::internal::_log_module_tag), __FILE__, __LINE__, __VA_ARGS__ )

#define LOG_ERROR( ... ) ::internal::log( ::internal::error, _get_log_module(::internal::_log_module_tag), __FILE__, __LINE__, __VA_ARGS__ )
#define LOG_WARNING( ... ) ::internal::log( ::internal::warning, _get_log_module(::internal::_log_module_tag), __FILE__, __LINE__, __VA_ARGS__ )
#define LOG_INFORMATION( ... ) ::internal::log( ::internal::information, _get_log_module(::internal::_log_module_tag), __FILE__, __LINE__, __VA_ARGS__ )
#define LOG_PERFORMANCE( ... ) ::internal::log( ::internal::performance, _get_log_module(::internal::_log_module_tag), __FILE__, __LINE__, __VA_ARGS__ )


// The second argument must be a string literal
#define FATAL_ASSERT( cond, ... ) (void)(!!(cond) ? 0 : ::internal::assert_failed(_get_log_module(::internal::_log_module_tag), __FILE__, __LINE__, #cond, __VA_ARGS__),0)
