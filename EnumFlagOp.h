#pragma once

#include <type_traits>

#define DECLARE_ENUM_FLAG( enum )                                   \
    inline static constexpr enum operator | ( enum e1, enum e2 ) {  \
        return static_cast<enum>(                                   \
            std::underlying_type<enum>::type(e1) |                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline static constexpr enum operator & ( enum e1, enum e2 ) {  \
        return static_cast<enum>(                                   \
            std::underlying_type<enum>::type(e1) &                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline static constexpr enum operator ^ ( enum e1, enum e2 ) {  \
        return static_cast<enum>(                                   \
            std::underlying_type<enum>::type(e1) ^                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline static constexpr enum operator ~ ( enum e1 ) {           \
        return static_cast<enum>(                                   \
            ~std::underlying_type<enum>::type(e1)                   \
        );                                                          \
    }                                                               \
    inline static constexpr enum operator |= ( enum &e1, enum e2 ) {\
        return e1 = static_cast<enum>(                              \
            std::underlying_type<enum>::type(e1) |                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline static constexpr enum operator &= ( enum &e1, enum e2 ) {\
        return e1 = static_cast<enum>(                              \
            std::underlying_type<enum>::type(e1) &                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline static constexpr enum operator ^= ( enum &e1, enum e2 ) {\
        return e1 = static_cast<enum>(                              \
            std::underlying_type<enum>::type(e1) ^                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline static constexpr bool any( enum e, enum flags ) {        \
        return std::underlying_type<enum>::type(e&flags) != 0;      \
    }                                                               \
    inline static constexpr bool all( enum e, enum flags ) {        \
        return (e&flags) == flags;                                  \
    }                                                               \
    inline static constexpr bool none( enum e, enum flags ) {       \
        return std::underlying_type<enum>::type(e&flags) == 0;      \
    }                                                               \
    inline static constexpr enum clear( enum e, enum flags ) {      \
        return e & (~flags);                                        \
    }                                                               \
    inline static constexpr enum set( enum e, enum flags ) {        \
        return e | flags;                                           \
    }                                                               \
    

#define ENUM_FLAG_VALUE(x) (1 << (x))