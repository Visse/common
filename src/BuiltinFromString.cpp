#include "BuiltinFromString.h"
#include "StringEqual.h"


#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdint>

namespace Common
{
    template< typename Type, typename std::enable_if<std::is_integral<Type>::value && !std::is_unsigned<Type>::value, int>::type = 0>
    Type fromString( const char *str, char **end )
    {
        return (Type)std::strtol(str, end, 10);
    }

    template< typename Type, typename std::enable_if<std::is_unsigned<Type>::value, int>::type = 0>
    Type fromString( const char *str, char **end )
    {
        return (Type)std::strtoul(str, end, 10);
    }
    
    template< typename Type, typename std::enable_if<std::is_floating_point<Type>::value, int>::type = 0>
    Type fromString( const char *str, char **end )
    {
        return (Type)std::strtof(str, end);
    }

    void removeSpaces( const char *&str, size_t &len )
    {
        while (std::isspace(*str) && len > 0) {
            str++;
            len--;
        }
        while (len > 0 && std::isspace(str[len-1])) --len;
    }

    template< typename Type >
    COMMON_API bool BuiltinFromString( const char *str, size_t len, Type &value )
    {
        removeSpaces(str, len);

        // Make sure the string is null terminated
        if (len > 31) return false;

        char buff[32];
        std::memcpy(buff, str, len);
        buff[len] = 0;
        
        char *end;
        value = fromString<Type>(buff, &end);
        return end == (buff+len);
    }

    template<>
    COMMON_API bool BuiltinFromString<bool>( const char *str, size_t len, bool &value ) 
    {
        removeSpaces(str, len);

        if (StringUtils::equal(str, len, "true", StringUtils::EqualFlags::IgnoreCase) ||
            StringUtils::equal(str, len, "1", StringUtils::EqualFlags::IgnoreCase)) 
        {
            value = true;
            return true;
        }
        if (StringUtils::equal(str, len, "false", StringUtils::EqualFlags::IgnoreCase) ||
            StringUtils::equal(str, len, "0", StringUtils::EqualFlags::IgnoreCase)) 
        {
            value = false;
            return true;
        }
        return false;
    }


    template COMMON_API bool BuiltinFromString<int>( const char *str, size_t len, int &value );
    template COMMON_API bool BuiltinFromString<unsigned>( const char *str, size_t len, unsigned &value );


    template COMMON_API bool BuiltinFromString<int32_t>( const char *str, size_t len, int32_t &value );
    template COMMON_API bool BuiltinFromString<uint32_t>( const char *str, size_t len, uint32_t &value );
    

    template COMMON_API bool BuiltinFromString<float>( const char *str, size_t len, float &value );
    template COMMON_API bool BuiltinFromString<double>( const char *str, size_t len, double &value );
    

}