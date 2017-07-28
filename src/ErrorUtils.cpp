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
    enum Color {
        Information,
        Warning,
        Error,
        Performance
    };
    struct ColorLock {
        ColorLock( Color color );
        ~ColorLock();
    };


    void logva( const char *format, std::va_list args ) {
        char buffer[STB_SPRINTF_MIN];

        STBSP_SPRINTFCB *callback = []( char *buf, void *user, int len) -> char* {
            fwrite(buf, len, 1, stderr);
            return buf;
        };

        stbsp_vsprintfcb( callback, nullptr, buffer, format, args );
    }
    void log( const char *format, ... ) {
        va_list args;
        va_start(args, format);
        logva(format, args);
        va_end(args);
    }

    void printTimeStamp() {
        uint64_t time = Clock::GetMSecSinceStart();
        
        unsigned msecs =  (unsigned)(time % 1000),
                 seconds = (unsigned)((time/1000) % 60),
                 minutes = (unsigned)(time/60000);
        
        log("[%0u:%02u:%03u]\t", minutes, seconds, msecs);
    }

    void NO_RETURN fatal_error( const char* format, ... )
    {
        {
            ColorLock lock( Error );
            printTimeStamp();
        

            va_list args;
            va_start( args, format );
            logva(format, args);
            std::fputc( '\n', stderr );
            va_end( args );
        }
        std::fflush( stderr );

        debugbreak;
        raise( SIGINT );
        std::exit( 1 );
    }
    
    void error(const char* format, ...)
    {
        ColorLock lock( Error );
        printTimeStamp();
        
        va_list args;
        va_start( args, format );
        logva(format, args);
        std::fputc( '\n', stderr );
        va_end( args );
        std::fflush( stderr );
        
//         raise( SIGTRAP );
    }

    void warning(const char* format, ...)
    {
        ColorLock lock( Warning );
        printTimeStamp();
        
        va_list args;
        va_start( args, format );
        logva(format, args);
        std::fputc( '\n', stderr );
        va_end( args );
        std::fflush( stderr );
        
//         raise( SIGTRAP );
    }
    
    void information(const char* format, ...)
    {
        ColorLock lock( Information );
        printTimeStamp();
        
        va_list args;
        va_start( args, format );
        logva(format, args);
        std::fputc( '\n', stderr );
        va_end( args );
        std::fflush( stderr );
    }
    
    void performance(const char* format, ...)
    {
        ColorLock lock( Performance );
        printTimeStamp();
        
        va_list args;
        va_start( args, format );
        logva(format, args);
        std::fputc( '\n', stderr );
        va_end( args );
        std::fflush( stderr );
    }

    void NO_RETURN assert_failed( const char *condition, const char *format, ... )
    {
        {
            ColorLock lock( Error );
            printTimeStamp();

            log("Assertion \"%s\" Failed: ", condition);
            
            va_list args;
            va_start(args, format);
            logva(format, args);
            std::fputc('\n', stderr);
            va_end(args);
        }
        std::fflush(stderr);
        
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

internal::ColorLock::ColorLock(internal::Color color)
{
    if( !COLOR_INIT ) {
        SUPPORTS_COLOR = isatty(fileno(stderr));
        COLOR_INIT = true;
    }
    if( !SUPPORTS_COLOR ) return;

    switch( color ) {
    case( Information ):
        break;
    case( Warning ):
        std::fputs( COLOR_YELLOW, stderr );
        break;
    case( Error ):
        std::fputs( COLOR_RED, stderr );
        break;
    case( Performance ):
        std::fputs( COLOR_BLUE, stderr );
        break;
    }
}

internal::ColorLock::~ColorLock()
{
    if( !SUPPORTS_COLOR ) return;

    std::fputs( COLOR_NORMAL, stderr );
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

internal::ColorLock::ColorLock(internal::Color color)
{
    if( !COLOR_INIT ) {
        hConsole = GetStdHandle( STD_ERROR_HANDLE );
        COLOR_INIT = true;

        SUPPORTS_COLOR = (hConsole != INVALID_HANDLE_VALUE);
    }
    if( !SUPPORTS_COLOR ) return;

    switch( color ) {
    case( Information ):
        break;
    case( Warning ):
        SetConsoleTextAttribute( hConsole, COLOR_YELLOW );
        break;
    case( Error ):
        SetConsoleTextAttribute( hConsole, COLOR_RED );
        break;
    case( Performance ):
        SetConsoleTextAttribute( hConsole, COLOR_BLUE );
        break;
    }
}

internal::ColorLock::~ColorLock()
{
    if( !SUPPORTS_COLOR ) return;
    
        SetConsoleTextAttribute( hConsole, COLOR_NORMAL );
}

#endif
