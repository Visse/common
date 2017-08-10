#pragma once


#ifndef ENUM_STRING_IMPL

/** Usage of DECL_ENUM_FROM_STRING **
    
    With ExampleEnum defined as
      >  enum ExampleEnum {
      >      Value1,
      >      Value2,
      >  }
    And DECL_ENUM_FROM_STRING called with:
      > DECL_ENUM_FROM_STRING( ExampleEnum, API, 
      >     (Value1),
      >     (Value2, "Alternative1", "Alternative2")
      > )
    A function is generated that is ekvevlent to
      > API bool ExampleEnumFromString( const char *str, size_t len, ExampleEnum &value )
      > {
      >     switch( str ) {  // If avalible will use a constexpr hash function otherwise fallback to a if-ifelse chain
      >     case "Value1":
      >         value =  ExampleEnum::Value1;
      >         return true;
      >     case "Value2":
      >     case "Alternative1":
      >     case "Alternative2":
      >         value = ExampleEnum::Value2;
      >         return true;
      >     }
      >     return false;
      > }
*/

#define DECL_ENUM_FROM_STRING(Enum, API, ...)                               \
    API bool Enum##FromString(const char *str, size_t len, Enum &value);    \
    API bool Enum##FromString(const char *str, Enum &value);

#define DECL_ENUM_TO_STRING(Enum, API, ...) \
    API const char *Enum##ToString(Enum value);

#else // ENUM_STRING_IMPL

#include "PPUtils.h"

namespace internal
{
    inline constexpr char tolowercase( char c ) {
        return (c >= 'A' && c <= 'Z') ? c - ('A'-'a') : c;
    }
    
    inline constexpr size_t hashLen( const char *str, size_t len, size_t seed = 14695981039346656037u )
    {
        return len > 0 ? hashLen(str+1, len-1, (seed ^ size_t(tolowercase(str[0]))) * 1099511628211u) : seed;
    }

    inline constexpr size_t hash( const char *str, size_t seed = 14695981039346656037u )
    {
        return str[0] ? hash(str+1, (seed ^ size_t(tolowercase(str[0]))) * 1099511628211u) : seed;
    }
}

#define DECL_ENUM_FROM_STRING_CASE_ALT(Name, Alt)       \
    case internal::hash(Alt):
#define DECL_ENUM_FROM_STRING_CASE(Name, ...)                       \
    case internal::hash(#Name):                                     \
    PP_UTILS_MAP(Name, DECL_ENUM_FROM_STRING_CASE_ALT, __VA_ARGS__) \
        value = EnumType::Name;                                     \
        return true;

#define DECL_ENUM_FROM_STRING_CASE_UNPACK(Enum, Arg)    \
    DECL_ENUM_FROM_STRING_CASE Arg

#define DECL_ENUM_FROM_STRING(Enum, API, ...)                                   \
    API bool Enum##FromString(const char *str, size_t len, Enum &value) {       \
        using EnumType = Enum;                                                  \
        switch(internal::hashLen(str, len)) {                                   \
            PP_UTILS_MAP(Enum, DECL_ENUM_FROM_STRING_CASE_UNPACK, __VA_ARGS__)  \
        }                                                                       \
        return false;                                                           \
    }                                                                           \
    API bool Enum##FromString(const char *str, Enum &value) {                   \
        return Enum##FromString(str, strlen(str), value);                       \
    }


#define DECL_ENUM_TO_STRING_CASE(Name, ...)   \
    case EnumType::Name:                      \
        return #Name;

#define DECL_ENUM_TO_STRING_CASE_UNPACK(Enum, Arg)  \
    DECL_ENUM_TO_STRING_CASE Arg

#define DECL_ENUM_TO_STRING(Enum, API, ...)                                     \
    API const char *Enum##ToString(Enum value) {                                \
        using EnumType = Enum;                                                  \
        switch (value) {                                                        \
            PP_UTILS_MAP(Enum, DECL_ENUM_TO_STRING_CASE_UNPACK, __VA_ARGS__)    \
        default:                                                                \
            return "Invalid " ## #Enum;                                         \
        }                                                                       \
    }

#endif // ENUM_STRING_IMPL


#define DECL_ENUM_TO_FROM_STRING(Enum, API, ...) \
    DECL_ENUM_FROM_STRING(Enum, API, __VA_ARGS__) \
    DECL_ENUM_TO_STRING(Enum, API, __VA_ARGS__)






