#include "ErrorUtils.h"
#include "Clock.h"

#include "stb_sprintf.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#include <signal.h>

#ifdef _MSC_VER
#define debugbreak __debugbreak()
#else
#define debugbreak (void)
#endif


namespace internal 
{
    // Helper for setting console color,
    // its also responseble for adding a new line and flushing
    struct ColorLock {
        ColorLock( int level );
        ~ColorLock();
    };


    void outputva( const char *format, std::va_list args ) {
        char buffer[STB_SPRINTF_MIN];

        STBSP_SPRINTFCB *callback = []( char *buf, void *user, int len) -> char* {
            fwrite(buf, len, 1, stderr);
            return buf;
        };
        stbsp_vsprintfcb(callback, nullptr, buffer, format, args);
    }
    void output( const char *format, ... ) PRINTF_FORMAT(1,2) {
        va_list args;

        va_start(args, format);
        outputva(format, args);
        va_end(args);
    }

    void doLogHeaders( int level, const char *module, const char *file, int line )
    {

        uint64_t time = Clock::GetMSecSinceStart();
        unsigned msecs =  (unsigned)(time % 1000),
                 seconds = (unsigned)((time/1000) % 60),
                 minutes = (unsigned)(time/60000);
        
       // output("[%u:%u:%u][%i][%s][%s:%i]: ", minutes, seconds, msecs, level, module, file, line);
        // for now don't print filename+line to console, it takes up too much space
        output("[%02u:%02u:%03u][%i][%s]: ", minutes, seconds, msecs, level, module);
    }

    void doLog( int level, const char *module, const char *file, int line, const char *format, std::va_list args )
    {
        ColorLock lock(level);

        doLogHeaders(level, module, file, line);
        outputva(format, args);
    }
    
    COMMON_API void NO_RETURN fatal_error( const char *module, const char *file, int line, const char *format, ... )
    {
        va_list args;
        va_start( args, format );
        doLog(fatal, module, file, line, format, args);
        va_end( args );

        debugbreak;
        raise( SIGINT );
        std::exit( 1 );
    }
    
    COMMON_API void log( int level, const char *module, const char *file, int line, const char *format, ... )
    {
        va_list args;
        va_start( args, format );
        doLog(level, module, file, line, format, args);
        va_end( args );
    }
    
    COMMON_API void NO_RETURN assert_failed( const char *module, const char *file, int line, const char *condition, const char *format, ... )
    {
        ColorLock lock(fatal);
        doLogHeaders(fatal, module, file, line);

        output("Assertion \"%s\" failed: ", condition);

        va_list args;
        va_start(args, format);
        outputva(format, args);
        va_end(args);

        debugbreak;
        raise( SIGINT );
        std::exit( 1 );
    }

    COMMON_API void NO_RETURN assert_failed( const char *module, const char *file, int line, const char *condition )
    {
        ColorLock lock(fatal);
        doLogHeaders(fatal, module, file, line);

        output("Assertion \"%s\" failed: ", condition);

        debugbreak;
        raise( SIGINT );
        std::exit( 1 );
    }
}


#ifdef __unix__

#define COLOR_NORMAL "\x1B[0m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_RED    "\x1B[31m"
#define COLOR_BLUE   "\x1B[34m"

bool COLOR_INIT = false;
bool SUPPORTS_COLOR = false;

#include <unistd.h>

internal::ColorLock::ColorLock( int color)
{
    if( !COLOR_INIT ) {
        SUPPORTS_COLOR = isatty(fileno(stderr));
        COLOR_INIT = true;
    }
    if( !SUPPORTS_COLOR ) return;

    switch( color ) {
    case( debug ):
    case( information ):
        break;
    case( performance ):
        std::fputs( COLOR_BLUE, stderr );
        break;
    case( warning ):
        std::fputs( COLOR_YELLOW, stderr );
        break;
    case( error ):
    case( fatal ):
        std::fputs( COLOR_RED, stderr );
        break;
    }
}

internal::ColorLock::~ColorLock()
{
    if( !SUPPORTS_COLOR ) return;

    std::fputs( COLOR_NORMAL, stderr );
    std::fputc('\n', stderr);
    std::fflush(stderr);
}

#elif WIN32

#include <Windows.h>

#define COLOR_NORMAL 15
#define COLOR_YELLOW 14
#define COLOR_RED    12
#define COLOR_BLUE   9


bool COLOR_INIT = false;
bool SUPPORTS_COLOR = false;

HANDLE hConsole;

internal::ColorLock::ColorLock( int color)
{
    if( !COLOR_INIT ) {
        hConsole = GetStdHandle( STD_ERROR_HANDLE );
        COLOR_INIT = true;

        SUPPORTS_COLOR = (hConsole != INVALID_HANDLE_VALUE);
    }
    if( !SUPPORTS_COLOR ) return;
    
    switch( color ) {
    case( debug ):
    case( information ):
        break;
    case( performance ):
        SetConsoleTextAttribute( hConsole, COLOR_BLUE );
        break;
    case( warning ):
        SetConsoleTextAttribute( hConsole, COLOR_YELLOW );
        break;
    case( error ):
    case( fatal ):
        SetConsoleTextAttribute( hConsole, COLOR_RED );
        break;
    }
}

internal::ColorLock::~ColorLock()
{
    if( !SUPPORTS_COLOR ) return;
    
    SetConsoleTextAttribute( hConsole, COLOR_NORMAL );
    std::fputc('\n', stderr);
    std::fflush(stderr);
}

#endif
