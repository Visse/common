#pragma once

#include "StringEqual.h"

#include <sstream>

namespace Common
{
    namespace internal
    {
        template< typename Type >
        struct GetHelper {
            static bool get( const char *str, Type &val ) {
                std::istringstream stream(str);
                stream >> val;
                return !!stream;
            }
        };

        template<>
        struct GetHelper<std::string> {
            static bool get( const char *str, std::string &val ) {
                val = str;
                return true;
            }
        };

        inline int findCommandLineArg( int argc, const char* const *argv, const char *name, StringUtils::EqualFlags equalFlags )
        {
            // 0  = program name, not argument
            for (int i=1; i < argc; ++i) {
                if (StringUtils::startsWith(argv[i], name, equalFlags)) return i;
            }

            return -1;
        }
    }

    inline bool hasCommandLineArg( int argc, const char* const *argv, const char *name, StringUtils::EqualFlags equalFlags )
    {
        return internal::findCommandLineArg(argc, argv, name, equalFlags) != -1;
    }

    template< typename Type >
    bool getCommandLineArg( int argc, const char* const *argv, const char *name, Type &val, StringUtils::EqualFlags equalFlags )
    {
        int idx = internal::findCommandLineArg(argc, argv, name, equalFlags);
        if (idx == -1) return false;

        size_t nameLen = std::strlen(name);
        const char *value = argv[idx] + nameLen;

        return internal::GetHelper<Type>::get(value, val);
    }

    class CommandLine {
        int argc;
        const char* const* argv;
        StringUtils::EqualFlags equalFlags = StringUtils::EqualFlags::None;

    public:
        CommandLine( int argc_, const char* const *argv_ ) :
            argc(argc_), argv(argv_)
        {}
        
        template< typename Type >
        bool get( const char *name, Type &val ) const {
            return getCommandLineArg(argc, argv, name, val, equalFlags);          
        }

        template< typename Type >
        bool get( const char *name, Type &val, const Type &default) const {
            if (getCommandLineArg(argc, argv, name, val, equalFlags)) {
                return true;
            }
            val = default;
            return false;
        }

        bool has( const char *name ) const {
            return hasCommandLineArg(argc, argv, name, equalFlags);
        }
    };
}